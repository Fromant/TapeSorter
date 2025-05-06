cmake_minimum_required(VERSION 3.30)
project(TapeSorter)

add_subdirectory(src)

set(CMAKE_CXX_STANDARD 20)

add_executable(TapeSorter
        TapeSorter.cpp
)

target_link_libraries(TapeSorter PRIVATE TapeSorterLib)

enable_testing()

#install gtest for testing
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.10.0
)

FetchContent_MakeAvailable(googletest)

add_executable(TapeSorterTests tests/tests.cpp)

target_link_libraries(TapeSorterTests PRIVATE gtest_main TapeSorterLib)



