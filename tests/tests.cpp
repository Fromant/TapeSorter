#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <vector>
#include <cstdlib>
#include <algorithm>

#include "../src/sorter/TapeSorter.h"
#include "../src/tapes/FileTape.h"

namespace fs = std::filesystem;

// Helper functions
void write_tape_file(const fs::path &path, const std::vector<int32_t> &data) {
    std::ofstream file(path, std::ios::binary | std::ios::out | std::ios::trunc);
    for (int32_t num: data) {
        file.write(reinterpret_cast<const char *>(&num), sizeof(num));
    }
}

std::vector<int32_t> read_tape_file(const fs::path &path) {
    std::ifstream file(path, std::ios::binary | std::ios::in);
    std::vector<int32_t> data;
    int32_t num;
    while (file.read(reinterpret_cast<char *>(&num), sizeof(num))) {
        data.push_back(num);
    }
    return data;
}

TapeSorter runSorter(const fs::path &input_path, const fs::path &output_path, const fs::path &config_ath) {
    FileTape input(input_path.string());
    FileTape output(output_path.string());

    Config config;
    config.loadFromFile(config_ath.string());
    TapeSorter sorter{input, output, config};
    sorter.sort();

    return sorter;
}

class TapeSorterTest : public ::testing::Test {
protected:
    static fs::path create_temp_dir() {
        fs::create_directories("./test_files/");
        return "./test_files/";
    }

    void SetUp() override {
        temp_dir = create_temp_dir();
    }

    void TearDown() override {
        fs::remove_all(temp_dir);
    }

    fs::path temp_dir;
};

TEST_F(TapeSorterTest, EmptyInput) {
    write_tape_file(temp_dir / "input.bin", {});

    // Create config
    std::ofstream(temp_dir / "config.ini") <<
            "memory_limit=4\nread_delay=1\nwrite_delay=1\nshift_delay=1\nrewind_delay=1";

    // Run program
    runSorter(temp_dir / "input.bin", temp_dir / "output.bin", temp_dir / "config.ini");

    // Verify output
    auto result = read_tape_file(temp_dir / "output.bin");
    EXPECT_TRUE(result.empty());
}

TEST_F(TapeSorterTest, SingleElement) {
    write_tape_file(temp_dir / "input.bin", {5});
    std::ofstream(temp_dir / "config.ini") <<
            "memory_limit=4\nread_delay=1\nwrite_delay=1\nshift_delay=1\nrewind_delay=1";

    // Run program
    auto sorter = runSorter(temp_dir / "input.bin", temp_dir / "output.bin", temp_dir / "config.ini");

    auto output = read_tape_file(temp_dir / "output.bin");
    ASSERT_EQ(output, std::vector<int32_t>({5}));

    EXPECT_GE(sorter.tapeReadCount, 1);
    EXPECT_GE(sorter.tapeWriteCount, 1);
    EXPECT_GE(sorter.tapeRewindCount, 0);
    EXPECT_GE(sorter.tapeShiftCount, 1);
}

TEST_F(TapeSorterTest, AlreadySorted) {
    std::vector<int32_t> data = {1, 2, 3, 4};
    write_tape_file(temp_dir / "input.bin", data);
    std::ofstream(temp_dir / "config.ini") <<
            "memory_limit=16\nread_delay=1\nwrite_delay=1\nshift_delay=1\nrewind_delay=1";

    runSorter(temp_dir / "input.bin", temp_dir / "output.bin", temp_dir / "config.ini");

    auto output = read_tape_file(temp_dir / "output.bin");
    EXPECT_EQ(data, output);
}

TEST_F(TapeSorterTest, ReverseSorted) {
    std::vector<int32_t> data = {4, 3, 2, 1};
    write_tape_file(temp_dir / "input.bin", data);
    std::ofstream(temp_dir / "config.ini") <<
            "memory_limit=16\nread_delay=1\nwrite_delay=1\nshift_delay=1\nrewind_delay=1";

    runSorter(temp_dir / "input.bin", temp_dir / "output.bin", temp_dir / "config.ini");

    auto output = read_tape_file(temp_dir / "output.bin");
    std::sort(data.begin(), data.end());
    EXPECT_EQ(data, output);
}

TEST_F(TapeSorterTest, MissingConfigFile) {
    write_tape_file(temp_dir / "input.bin", {1, 2, 3});
    // No config.ini created

    Config config;
    EXPECT_EQ(config.loadFromFile(temp_dir.string() + "config.ini"), false);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
