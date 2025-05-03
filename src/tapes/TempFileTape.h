#ifndef TEMPFILETAPE_H
#define TEMPFILETAPE_H
#include <cstdint>
#include <fstream>
#include <string>

#include "Tape.h"
#include "../config/Config.h"


/// Class for creating a temporary file tape.
/// Very helpful for creating temporary files as them should trunc content on opening
/// Important to mention that all of this is needed only because of that fact that we can't just
/// have a million of FilTape objects living. More details inside TapeSorter.cpp.sort()
class TempFileTape : public Tape {
public:
    TempFileTape(const std::string &filename, const Config &config);

    ~TempFileTape() override;

    int32_t read() override;

    void write(int32_t value) override;

    void shiftLeft() override;

    void shiftRight() override;

    void rewind() override;

    bool hasNext() override;

    int getPosition() override {
        return position;
    }

private:
    std::string filename;
    size_t position;
    size_t size;
    std::fstream file;
    const Config &config;
};


#endif //TEMPFILETAPE_H
