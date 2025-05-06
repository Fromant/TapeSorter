#ifndef TAPE_SORTER_H
#define TAPE_SORTER_H


#include "../tapes/Tape.h"
#include "../config/Config.h"


class TapeSorter {
public:
    TapeSorter(Tape &input, Tape &output, const Config &config);

    /// Call this to sort input tape into output tape,
    /// Will create a lot of temporary tapes in tmp directory
    void sort();

    int tapeShiftCount = 0;
    int tapeRewindCount = 0;
    int tapeReadCount = 0;
    int tapeWriteCount = 0;

private:
    Tape &input_tape;
    Tape &output_tape;
    int temp_tapes = 0;

    const Config &config;

    /// Merge tapes from where they are (intended as end of tape) to their start
    /// @param tape1 First tape to source from
    /// @param tape2 Second tape to source from
    /// @param resultTape Tape where results will go
    void mergeTapesBackwards(Tape &tape1, Tape &tape2, Tape &resultTape);

    /// Smart rewinding:
    /// We shouldn't call rewind() if it's easier to shift n times to the tape's start
    /// @param tape Tape to perform rewind on
    void rewindTape(Tape &tape);

    void k_way_sort(int &merged_tapes, Tape& output);
};

#endif // TAPE_SORTER_H
