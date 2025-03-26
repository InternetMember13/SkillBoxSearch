#ifndef INVERTED_INDEX_H 
#define INVERTED_INDEX_H 

#include <unordered_map> 
#include <vector> 
#include <string> 
#include <fstream> 
#include <sstream> 
#include <iostream> 

class FileIndexer { 
public: 
    FileIndexer() = default; 
    FileIndexer(const std::vector<std::string>& files); 

    const std::unordered_map<std::string, std::vector<std::string>>& get_index() const; 

private: 
    void inverted_index(); 
    void index_file(const std::string& filename, const std::string& content); 
    std::string process_content(const std::string& content); 

    std::vector<std::string> files; 
    std::unordered_map<std::string, std::vector<std::string>> index; 
}; 

std::vector<std::string> process_query(const std::string& query); 

#endif    

