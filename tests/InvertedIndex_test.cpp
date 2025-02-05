#include <gtest/gtest.h>
#include "Inverted_index.h"  

using namespace std;
void create_temp_file(const string& filename, const string& content) {
    ofstream ofs(filename);
    ofs << content;
    ofs.close();
}

TEST(FileIndexerTest, TestBasicIndexing) {
    
    create_temp_file("file1.txt", "london is the capital of great the britain");
    create_temp_file("file2.txt", "big ben is the nickname for Great bell of striking clock");

    vector<string> files = {"file1.txt", "file2.txt"};
    FileIndexer indexer(files, "low");  

    const auto& index = indexer.get_index();

    ASSERT_EQ(index.at("london").size(), 1);
    ASSERT_EQ(index.at("the").size()-2, 1);
    remove("file1.txt");
    remove("file2.txt");
}

TEST(FileIndexerTest, TestCaseInsensitiveIndexing) {
    create_temp_file("file1.txt", "Milk milk milk milk");
    create_temp_file("file2.txt", "water water WATER");

    vector<string> files = {"file1.txt", "file2.txt"};
    FileIndexer indexer(files, "low");  

    const auto& index = indexer.get_index();

    ASSERT_EQ(index.at("milk").size(), 4);
    ASSERT_EQ(index.at("milk")[0], "file1.txt");
    ASSERT_EQ(index.at("water").size(), 3);
    ASSERT_EQ(index.at("water")[0], "file2.txt");
    remove("file1.txt");
    remove("file2.txt");
}

TEST(FileIndexerTest, TestNonExistentFile) {
    vector<string> files = {"nonexistent_file.txt"};
    FileIndexer indexer(files, "low");  

    const auto& index = indexer.get_index();
    
    ASSERT_TRUE(index.empty());
}

TEST(FileIndexerTest, TestMultipleOccurrences) {

    create_temp_file("file1.txt", "milk milk milk water");

    vector<string> files = {"file1.txt"};
    FileIndexer indexer(files, "low");  

    const auto& index = indexer.get_index();

    ASSERT_EQ(index.at("milk").size(), 3);
    ASSERT_EQ(index.at("milk")[0], "file1.txt");
    ASSERT_EQ(index.at("water").size(), 1);
    ASSERT_EQ(index.at("water")[0], "file1.txt");

    remove("file1.txt");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
