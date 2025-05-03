#include "TapeSorter.h"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <vector>

#include "../tapes/FileTape.h"
#include "../config/Config.h"
#include "../tapes/TempFileTape.h"

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

    // read and merge sort each 2*M (memory limit)
    // by merging 2 tapes on read we have 2+N/M/2 temp tapes instead of N/M temp tapes
    // therefore, we save 2*rewinding time when sorting. Pure win
    while (input_tape.hasNext()) {
        TempFileTape tape1{"tmp/tape_input1.bin", config};
        TempFileTape tape2{"tmp/tape_input2.bin", config};
        for (size_t i = 0; i < blockSize && input_tape.hasNext(); ++i) {
            block.push_back(input_tape.read());
            input_tape.shiftRight();
        }
        std::sort(block.begin(), block.end());

        for (int num: block) {
            tape1.write(num);
            tape1.shiftRight();
        }
        block.clear();
        for (size_t i = 0; i < blockSize && input_tape.hasNext(); ++i) {
            block.push_back(input_tape.read());
            input_tape.shiftRight();
        }
        std::sort(block.begin(), block.end());
        for (int num: block) {
            tape2.write(num);
            tape2.shiftRight();
        }
        block.clear();

        TempFileTape resultTape{"tmp/tape" + std::to_string(this->temp_tapes++) + ".bin", config};

        mergeTwoTapes(tape1, tape2, resultTape);
    }

    // merge sort all the tapes
    int merged = 0;
    while (merged < temp_tapes - 1) {
        int tapes_before = temp_tapes - merged;
        for (size_t i = 0; i < tapes_before; i += 2) {
            if (i + 1 < tapes_before) {
                FileTape tape1{"tmp/tape" + std::to_string(i) + ".bin", config};
                FileTape tape2{"tmp/tape" + std::to_string(i + 1) + ".bin", config};
                TempFileTape result{"tmp/tape" + std::to_string(temp_tapes++) + ".bin", config};
                mergeTwoTapes(tape1, tape2, result);
            } else {
                FileTape source{"tmp/tape" + std::to_string(i) + ".bin", config};
                TempFileTape result{"tmp/tape" + std::to_string(temp_tapes++) + ".bin", config};
                copyTape(source, result);
            }
        }
        merged += tapes_before;
    }

    FileTape result{"tmp/tape" + std::to_string(merged) + ".bin", config}; //get last tape
    copyTape(result, output_tape);
}

void TapeSorter::mergeTwoTapes(Tape &tape1, Tape &tape2, Tape &resultTape) {
    //TODO move backwards if tape is in it's last pos
    tape1.rewind();
    tape2.rewind();
    resultTape.rewind();

    bool has1 = tape1.hasNext();
    bool has2 = tape2.hasNext();
    int val1 = has1 ? tape1.read() : 0;
    int val2 = has2 ? tape2.read() : 0;

    while (has1 || has2) {
        if (has1 && has2) {
            if (val1 <= val2) {
                resultTape.write(val1);
                tape1.shiftRight();
                has1 = tape1.hasNext();
                if (has1) val1 = tape1.read();
            } else {
                resultTape.write(val2);
                tape2.shiftRight();
                has2 = tape2.hasNext();
                if (has2) val2 = tape2.read();
            }
        } else if (has1) {
            resultTape.write(val1);
            tape1.shiftRight();
            has1 = tape1.hasNext();
            if (has1) val1 = tape1.read();
        } else {
            resultTape.write(val2);
            tape2.shiftRight();
            has2 = tape2.hasNext();
            if (has2) val2 = tape2.read();
        }
        resultTape.shiftRight();
    }
}

void TapeSorter::copyTape(Tape &src, Tape &dest) {
    src.rewind();
    dest.rewind();
    while (src.hasNext()) {
        int val = src.read();
        dest.write(val);
        dest.shiftRight();
        src.shiftRight();
    }
}
