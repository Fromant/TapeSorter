#include "config.h"
#include <fstream>
#include <sstream>

bool Config::loadFromFile(const std::string &filename) {
    std::ifstream file(filename);
    if (!file) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        if (std::getline(iss, key, '=')) {
            int value;
            if (iss >> value) {
                if (key == "read_delay") read_delay = value;
                else if (key == "write_delay") write_delay = value;
                else if (key == "shift_delay") shift_delay = value;
                else if (key == "rewind_delay") rewind_delay = value;
                else if (key == "memory_limit") memory_limit = value;
            }
        }
    }

    return true;
}
