#include <gtest/gtest.h>
#include "ConfigLoader.h" 

class ConfigLoaderTest : public ::testing::Test {
protected:
    ConfigLoader* config_loader;

    void SetUp() override {
        std::ofstream test_file("test_config.json");
        test_file << R"({
            "files": ["file1.txt", 
                      "file2.txt"],
            "config": {
                "max_responses": 5
            }
        })";
        test_file.close();
        config_loader = new ConfigLoader("test_config.json");
    }

    void TearDown() override {
        delete config_loader;
        std::remove("test_config.json"); 
    }
};

TEST_F(ConfigLoaderTest, LoadFiles) {
    const auto& files = config_loader->get_files();
    EXPECT_EQ(files.size(), 2);
    EXPECT_EQ(files[0], "file1.txt");
    EXPECT_EQ(files[1], "file2.txt");
}

TEST_F(ConfigLoaderTest, LoadMaxResponses) {
    EXPECT_EQ(config_loader->get_max_responses(), 5);
}
