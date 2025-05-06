cmake_minimum_required(VERSION 3.30)
project(TapeSorter)

set(CMAKE_CXX_STANDARD 20)

add_executable(TapeSorter
        src/tapes/Tape.h
        src/tapes/FileTape.cpp
        src/tapes/FileTape.h
        src/config/Config.h
        src/config/Config.cpp
        src/sorter/TapeSorter.h
        src/sorter/TapeSorter.cpp
        tests/tests.cpp
)

#install gtest for testing
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.10.0
)

FetchContent_MakeAvailable(googletest)

target_link_libraries(TapeSorter PRIVATE gtest_main)

