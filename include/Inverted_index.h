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
    FileIndexer(const std::vector<std::string>& files, const std::string& formula)
        : files(files), formula(formula) {
        inverted_index();
    }

    const std::unordered_map<std::string, std::vector<std::string>>& get_index() const {
        return index;
    }

void inverted_index() {
        for (const auto& filename : files) {
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cerr << "File " << filename << " not found." << std::endl;
                continue;
            }
            std::stringstream index_stream;
            index_stream << file.rdbuf();
            std::string content = index_stream.str();
            index_file(filename, content);
        }
    }

    void index_file(const std::string& filename, const std::string& content) {
        std::string processed_content = process_content(content);
        std::istringstream iss(processed_content);
        std::string word;
        while (iss >> word) {
            index[word].push_back(filename);
        }
    }

    std::string process_content(const std::string& content) {
        if (formula == "low") {
            std::string lower_content = content;
            for (auto& c : lower_content) {
                c = tolower(c);
            }
            return lower_content;
        }
        return content;
    }

private:
    std::vector<std::string> files;
    std::string formula;
    std::unordered_map<std::string, std::vector<std::string>> index;
};
std::vector<std::string> process_query(const std::string& query) {
        std::istringstream iss(query);
        std::vector<std::string> words;
        std::string word;
        while (iss >> word) {
            words.push_back(word);
        }
        return words;
    }
#endif 
