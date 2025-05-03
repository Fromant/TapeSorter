#ifndef FILETAPE_H
#define FILETAPE_H

#include <cstdint>
#include <fstream>

#include "Tape.h"
#include "../config/Config.h"

/// File tape class, extends Tape interface.
/// Make sure you don't have too much of this created, because of each of them means
/// that std::fstream is opened and there's a limit of file descriptors opened for each process (512 on my windows)
class FileTape : public Tape {
public:
    FileTape(const std::string &filename, const Config &config);

    ~FileTape() override;

    int32_t read() override;

    void write(int32_t value) override;

    void shiftLeft() override;

    void shiftRight() override;

    void rewind() override;

    bool hasNext() override;

    int getPosition() override {
        return position;
    }

    int getSize() const { return size; }

private:
    std::string filename;
    //position and size in int32_t elements
    size_t position;
    size_t size;
    std::fstream file;
    const Config &config;
};


#endif //FILETAPE_H
