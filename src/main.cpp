#include "tapes/FileTape.h"
#include "sorter/TapeSorter.h"
#include "config/Config.h"
#include <iostream>

int main(int argc, char *argv[]) {
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
        FileTape inputTape(argv[1]);
        FileTape outputTape(argv[2], true);
        TapeSorter sorter(inputTape, outputTape, config);
        sorter.sort();

        //print statistics
        std::cout << "Done!\nInput file size: " << inputTape.getSize() << std::endl;
        std::cout << "Rewind count: " << sorter.tapeRewindCount << std::endl;
        std::cout << "Read count: " << sorter.tapeReadCount << std::endl;
        std::cout << "Write count: " << sorter.tapeWriteCount << std::endl;
        std::cout << "Shift count: " << sorter.tapeShiftCount << std::endl;
        std::cout << "Total time: " << sorter.tapeReadCount * config.read_delay +
                sorter.tapeWriteCount * config.write_delay +
                sorter.tapeShiftCount * config.shift_delay +
                sorter.tapeRewindCount * config.rewind_delay << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }


    return 0;
}
