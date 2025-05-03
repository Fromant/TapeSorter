#include "FileTape.h"

FileTape::FileTape(const std::string &filename, const Config &config)
    : filename(filename), position(0), config(config) {
    file.open(filename,
              std::ios::in | std::ios::out | std::ios::binary);
    if (!file) {
        file.open(filename, std::ios::out);
        file.close();
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        size = 0;
    } else {
        file.seekg(0, std::ios::end);
        size = file.tellg() / sizeof(int32_t);
    }
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
        //TODO remove throw
        throw std::runtime_error("Read error at position " + std::to_string(position));
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
    if (position == 0) return;
    position--;
}

void FileTape::rewind() {
    position = 0;
}

bool FileTape::hasNext() {
    return position < size;
}
