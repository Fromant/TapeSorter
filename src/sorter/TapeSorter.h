#ifndef TAPE_SORTER_H
#define TAPE_SORTER_H


#include "../tapes/FileTape.h"
#include "../tapes/Tape.h"


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

    /// Merge 2 tape while sorting their contents (without locating data in RAM)
    /// Does not rewind tapes and goes from where they're positioned to their end
    /// @param tape1 First tape to source from
    /// @param tape2 Second tape to source from
    /// @param resultTape Tape where results will go
    void mergeTapes(Tape &tape1, Tape &tape2, Tape &resultTape);

    /// Exactly the same as mergeTapes() but works from end of tape
    /// @param tape1 First tape to source from
    /// @param tape2 Second tape to source from
    /// @param resultTape Tape where results will go
    void mergeTapesBackwards(Tape &tape1, Tape &tape2, Tape &resultTape);


    /// Copies tape from where it's positioned to the end to other tape
    /// No rewinding here
    /// @param src Tape to copy from
    /// @param dest Copy destination
    void copyTape(Tape &src, Tape &dest);

    /// Smart rewinding:
    /// We shouldn't call rewind() if it's easier to shift n times to the tape's start
    /// @param tape Tape to perform rewind on
    void rewindTape(Tape &tape);
};

#endif // TAPE_SORTER_H
