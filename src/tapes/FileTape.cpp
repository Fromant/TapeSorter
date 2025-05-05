#include "FileTape.h"

#include <iostream>

FileTape::FileTape(const std::string &filename, bool isNew)
    : filename(filename), position(0) {
    file.open(filename,
              std::ios::in | std::ios::out | std::ios::binary | (isNew ? std::ios::trunc : (std::ios::openmode) 0));
    if (!file) {
        file.open(filename, std::ios::out);
        file.close();
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        size = 0;
    } else if (!isNew) {
        file.seekg(0, std::ios::end);
        size = file.tellg() / sizeof(int32_t);
    } else size = 0;

    if (!file) {
        throw std::runtime_error("Cannot open file " + filename);
    }
}

FileTape::~FileTape() {
    file.flush();
    file.close();
}

int32_t FileTape::read() {
    int32_t value = 0;
    file.seekg(position * sizeof(int32_t));
    if (!file.read(reinterpret_cast<char *>(&value), sizeof(int32_t))) {
        std::cerr << "Failed reading at position " << std::to_string(position) << " of " << size << ". Filename: "
                << filename << std::endl;
        // exit(-1);
    }
    return value;
}

void FileTape::write(int32_t value) {
    file.seekp(position * sizeof(int32_t));
    file.write(reinterpret_cast<const char *>(&value), sizeof(int32_t));
}

void FileTape::shiftRight() {
    position++;
    size = std::max(size, position);
}

void FileTape::shiftLeft() {
    position--;
}

void FileTape::rewind() {
    position = 0;
}

bool FileTape::hasNext() {
    return position < size;
}

bool FileTape::hasPrev() {
    return position > 0;
}
