#ifndef FILETAPE_H
#define FILETAPE_H

#include <cstdint>
#include <fstream>

#include "Tape.h"

/// File tape class, extends Tape interface.
/// Make sure you don't have too much of this created, because of each of them means
/// that std::fstream is opened and there's a limit of file descriptors opened for each process (512 on my windows)
class FileTape : public Tape {
public:
    /// Constructor :D
    /// @param filename filename where tape is be located in
    /// @param isNew if true, opens file truncating contents
    explicit FileTape(const std::string &filename, bool isNew = false);

    //move constructor and assignment for std::vector since std::fstream explicitly deletes them
    FileTape(FileTape &&other) noexcept {
        filename = other.filename;
        position = other.position;
        size = other.size;
        other.file.close();
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    }
    FileTape &operator=(FileTape &&other) noexcept {
        filename = other.filename;
        position = other.position;
        size = other.size;
        other.file.close();
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);

        return *this;
    }

    ~FileTape() override;

    int32_t read() override;

    void write(int32_t value) override;

    void shiftLeft() override;

    void shiftRight() override;

    void rewind() override;

    bool hasNext() override;

    bool hasPrev() override;

    int getPosition() override {
        return position;
    }

    void setPosition(size_t pos) { position = pos; }

    int getSize() const { return size; }

private:
    std::string filename;
    //position and size in int32_t elements
    size_t position;
    size_t size;
    std::fstream file;
};


#endif //FILETAPE_H
