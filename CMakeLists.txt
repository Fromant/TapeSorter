cmake_minimum_required(VERSION 3.30)
project(TapeSorter)

set(CMAKE_CXX_STANDARD 20)

add_executable(TapeSorter src/main.cpp
        src/tapes/Tape.h
        src/tapes/FileTape.cpp
        src/tapes/FileTape.h
        src/config/Config.h
        src/config/Config.cpp
        src/sorter/TapeSorter.h
        src/sorter/TapeSorter.cpp
        src/tapes/TempFileTape.cpp
        src/tapes/TempFileTape.h)
