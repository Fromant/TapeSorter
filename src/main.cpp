#include "tapes/FileTape.h"
#include "sorter/TapeSorter.h"
#include "config/Config.h"
#include <iostream>

int main(int argc, char* argv[]) {

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input file> <output file>\n";
        return 1;
    }

    const std::string config_path = "./config.ini";
    Config config;
    if (!config.loadFromFile(config_path)) {
        std::cerr << "Error loading config. Is it in " << config_path << '?' << std::endl;
        return 1;
    }

    try {
        FileTape inputTape(argv[1], config);
        FileTape outputTape(argv[2], config);
        TapeSorter sorter(inputTape, outputTape, config);
        sorter.sort();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}