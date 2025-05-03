#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct Config {
    int read_delay = 0;
    int write_delay = 0;
    int shift_delay = 0;
    int rewind_delay_per_position = 0;
    int memory_limit = 0;

    /// Function to load config from provided filename
    /// @param filename The filename of config file
    /// @return true if loaded successfully, false otherwise
    bool loadFromFile(const std::string &filename);
};

#endif //CONFIG_H
