#include "TapeSorter.h"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <vector>

#include "../tapes/FileTape.h"
#include "../config/Config.h"

TapeSorter::TapeSorter(Tape &input, Tape &output, const Config &config)
    : input_tape(input), output_tape(output), config(config) {
    std::filesystem::create_directory("tmp");
}

void TapeSorter::sort() {
    // It is important to say that we're creating and deleting a lot of temporary FileTape objects
    // instead of saving them in vector and reusing because of each living FileTape object
    // means 1 file descriptor opened, and this value is capped on some platforms. When testing, i found that
    // on my system cap is 512 opened descriptors.
    // More info: https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setmaxstdio?view=msvc-170

    size_t blockSize = config.memory_limit / sizeof(int32_t);
    std::vector<int> block;
    block.reserve(blockSize);

    input_tape.rewind();

    int n = 0;

    // read and merge sort each 2*M (memory limit)
    // by merging 2 tapes on read we have 2+N/M/2 temp tapes instead of N/M temp tapes
    // therefore, we save 2*rewinding time when sorting. Pure win
    while (input_tape.hasNext()) {
        FileTape tape1{"tmp/tape_input1.bin", true};
        FileTape tape2{"tmp/tape_input2.bin", true};
        for (size_t i = 0; i < blockSize && input_tape.hasNext(); ++i) {
            block.push_back(input_tape.read());
            tapeReadCount++;
            input_tape.shiftRight();
            tapeShiftCount++;
            n++;
        }
        std::sort(block.begin(), block.end());

        for (int num: block) {
            tape1.write(num);
            tapeWriteCount++;
            tape1.shiftRight();
            tapeShiftCount++;
        }
        block.clear();
        for (size_t i = 0; i < blockSize && input_tape.hasNext(); ++i) {
            block.push_back(input_tape.read());
            tapeReadCount++;
            input_tape.shiftRight();
            tapeShiftCount++;
        }
        std::sort(block.begin(), block.end());
        for (int num: block) {
            tape2.write(num);
            tapeWriteCount++;
            tape2.shiftRight();
            tapeShiftCount++;
        }
        block.clear();

        FileTape resultTape{"tmp/tape" + std::to_string(this->temp_tapes++) + ".bin", true};

        mergeTapesBackwards(tape1, tape2, resultTape);
    }

    int merged = 0;
    const int k = static_cast<int>(config.memory_limit / sizeof(int32_t));
    while (merged < temp_tapes - k - 1) {
        auto tape = FileTape("tmp/tape" + std::to_string(temp_tapes) + ".bin");
        k_way_sort(merged, tape);
        temp_tapes++;
    }

    //final merge
    k_way_sort(merged, output_tape);
}

void TapeSorter::mergeTapesBackwards(Tape &tape1, Tape &tape2, Tape &resultTape) {
    tape1.shiftLeft();
    tape2.shiftLeft();
    size_t pos1 = tape1.getPosition();
    size_t pos2 = tape2.getPosition();
    tapeShiftCount += 2;

    int val1 = 0, val2 = 0;
    if (pos1 != -1) {
        val1 = tape1.read();
        tapeReadCount++;
    }
    if (pos2 != -1) {
        val2 = tape2.read();
        tapeReadCount++;
    }

    while (pos2 != -1 || pos1 != -1) {
        if (pos1 != -1 && pos2 != -1) {
            if (val1 > val2) {
                resultTape.write(val1);
                tapeWriteCount++;
                tape1.shiftLeft();
                tapeShiftCount++;
                pos1 = tape1.getPosition();
                if (pos1 != -1) {
                    val1 = tape1.read();
                    tapeReadCount++;
                }
            } else {
                resultTape.write(val2);
                tapeWriteCount++;
                tape2.shiftLeft();
                tapeShiftCount++;
                pos2 = tape2.getPosition();
                if (pos2 != -1) {
                    val2 = tape2.read();
                    tapeReadCount++;
                }
            }
        } else if (pos1 != -1) {
            resultTape.write(val1);
            tapeWriteCount++;
            tape1.shiftLeft();
            tapeShiftCount++;
            pos1 = tape1.getPosition();
            if (pos1 != -1) {
                val1 = tape1.read();
                tapeReadCount++;
            }
        } else {
            resultTape.write(val2);
            tapeWriteCount++;
            tape2.shiftLeft();
            tapeShiftCount++;
            pos2 = tape2.getPosition();
            if (pos2 != -1) {
                val2 = tape2.read();
                tapeReadCount++;
            }
        }
        resultTape.shiftRight();
        tapeShiftCount++;
    }
}

void TapeSorter::rewindTape(Tape &tape) {
    auto pos = tape.getPosition();
    if (pos * config.shift_delay < config.rewind_delay) tapeShiftCount += pos;
    else tapeRewindCount++;

    tape.rewind();
}


/// Simple max function that return index of max value of given vector
/// @param v std::vector<int> to get maximum value of
/// @return Pair of index and value of maximum element in vector.
std::pair<size_t, int> min(const std::vector<int> &v) {
    int min = INT_MAX;
    size_t tr = 0;
    for (int i = 0; i < v.size(); i++) {
        if (v[i] < min) {
            min = v[i];
            tr = i;
        }
    }

    return {tr, min};
}

/// Function that sorts all temp tapes created
/// Result goes into new temporary tape or final output tape
/// @param merged_tapes amount of tapes already merged
void TapeSorter::k_way_sort(int &merged_tapes, Tape &output) {
    // we are limited in ram so we're having M/4-way merge sort here
    // in other words, K=M/4 since sizeof(int32)=4
    // Don't forget to limit number of tapes to sort
    const int K = std::min(temp_tapes - merged_tapes, static_cast<int>(config.memory_limit / sizeof(int32_t)));

    //Open all tapes
    std::vector<FileTape> tapes;
    tapes.reserve(K);
    for (int j = merged_tapes; j < K + merged_tapes; j++) {
        tapes.emplace_back("tmp/tape" + std::to_string(j) + ".bin", false);
        //assume that tape was closed at it's end
        if (tapes[j].getSize() == 0) continue;
        tapes[j].setPosition(tapes[j].getSize() - 1);
        tapeShiftCount++;
    }

    //Vector for memorizing all current values in tapes by their index.
    std::vector<int32_t> values;
    values.reserve(K);

    int tapes_merged_in_batch = 0;

    //read first values
    for (int j = merged_tapes; j < K; j++) {
        values.emplace_back(tapes[j].read());
        tapeReadCount++;
        tapes[j].shiftLeft();
        tapeShiftCount++;
    }

    while (tapes_merged_in_batch < K) {
        auto max_value = min(values);

        output.write(max_value.second);
        output.shiftRight();
        tapeWriteCount++;
        tapeShiftCount++;

        if (tapes[max_value.first].getPosition() == -1) {
            tapes_merged_in_batch++;
            values[max_value.first] = INT_MAX;
        } else {
            values[max_value.first] = tapes[max_value.first].read();
            tapes[max_value.first].shiftLeft();
            tapeShiftCount++;
            tapeReadCount++;
        }
    }

    merged_tapes += K;
}
