#ifndef TAPE_SORTER_H
#define TAPE_SORTER_H


#include "../tapes/FileTape.h"
#include "../tapes/Tape.h"


class TapeSorter {
public:
    TapeSorter(Tape &input, Tape &output, const Config &config);

    void sort();

private:
    Tape &input_tape;
    Tape &output_tape;
    int temp_tapes = 0;

    const Config &config;

    static void mergeTwoTapes(Tape &tape1, Tape &tape2, Tape &resultTape);
    static void copyTape(Tape &src, Tape &dest);
};

#endif // TAPE_SORTER_H
