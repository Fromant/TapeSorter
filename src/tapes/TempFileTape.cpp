#include "TempFileTape.h"

TempFileTape::TempFileTape(const std::string &filename, const Config &config) : config(config), filename(filename) {
    file.open(filename, std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
    if (!file) {
        file.open(filename, std::ios::out);
        file.close();
        file.open(filename, std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
    }
    size = 0;
    position = 0;
}

TempFileTape::~TempFileTape() {
    file.flush();
    file.close();
}

int32_t TempFileTape::read() {
    int32_t value = 0;
    file.seekg(position * sizeof(int32_t));
    if (!file.read(reinterpret_cast<char *>(&value), sizeof(int32_t))) {
        //TODO remove throw on release
        throw std::runtime_error("Read error at position " + std::to_string(position));
    }
    return value;
}

void TempFileTape::write(int32_t value) {
    file.seekp(position * sizeof(int32_t));
    file.write(reinterpret_cast<const char *>(&value), sizeof(int32_t));
}

void TempFileTape::rewind() {
    position = 0;
}

bool TempFileTape::hasNext() {
    return position < size;
}

void TempFileTape::shiftLeft() {
    position -= 1;
}

void TempFileTape::shiftRight() {
    position += 1;
    size = std::max(size, position);
}
