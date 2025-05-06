cmake_minimum_required(VERSION 3.30)
project(TapeSorterLib)

set(CMAKE_CXX_STANDARD 20)

add_library(TapeSorterLib
        tapes/Tape.h
        tapes/FileTape.cpp
        tapes/FileTape.h
        config/Config.h
        config/Config.cpp
        sorter/TapeSorter.h
        sorter/TapeSorter.cpp
)