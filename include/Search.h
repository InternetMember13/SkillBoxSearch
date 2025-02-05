#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <set>
#include <nlohmann/json.hpp>
#include <mutex>
#include <future>
#include "Inverted_index.h"
#include "ConfigLoader.h"
using json = nlohmann::json;
std::mutex mutex_threads;
struct WordCountResult {
    int total_word_count;
    std::map<std::string, int> requested_word_frequencies;
};
class SearchServer {
public:
    SearchServer(const ConfigLoader& config_loader)
        : files(config_loader.get_files()),
          formula(config_loader.get_formula()),
          max_responses(config_loader.get_max_responses()),
          indexer(FileIndexer(files, formula)) {}
    WordCountResult count_word_frequencies(const std::string& file_path, const std::vector<std::string>& requested_words) {
        std::ifstream file(file_path);
        std::string word;
        WordCountResult result = {0, {}};
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file " << file_path << std::endl;
            return result;
        }
        std::vector<std::string> lower_requested_words = requested_words;
        for (auto& words : lower_requested_words) {
            std::transform(words.begin(), words.end(), words.begin(), ::tolower);
        }
        while (file >> word) {
            word.erase(remove_if(word.begin(), word.end(), ispunct), word.end());
            if (!word.empty()) {
                std::transform(word.begin(), word.end(), word.begin(), ::tolower);
                result.total_word_count++;
                if (std::find(lower_requested_words.begin(), lower_requested_words.end(), word) != lower_requested_words.end()) {
                    result.requested_word_frequencies[word]++;
                }
            }
        }
        return result;
    }
void search(const std::vector<std::string>& queries) {
    std::ofstream file("../config/answers.json");
    json results = json::array();
    int id = 1;
    int global_max_word_count = 0; 

    for (const auto& query : queries) {
        std::vector<std::string> processed_queries = process_query(query);
        if (processed_queries.empty()) {
            std::cerr << "Error: No valid words in the query: " << query << std::endl;
            continue;
        }

        std::set<std::string> relevant_docs;
        auto it = indexer.get_index().find(processed_queries[0]);
        if (it != indexer.get_index().end()) {
            for (const auto& doc : it->second) {
                relevant_docs.insert(doc);
            }
        }

        json query_result = {
            {"result_found", false},
            {"results" + std::to_string(id), json::array()} 
        };

        std::vector<std::pair<std::string, int>> doc_word_counts; 

        for (const auto& doc : relevant_docs) {
            WordCountResult word_count_result = count_word_frequencies(doc, processed_queries);
            doc_word_counts.emplace_back(doc, word_count_result.total_word_count);
            
            if (word_count_result.total_word_count > global_max_word_count) {
                global_max_word_count = word_count_result.total_word_count;
            }
        }

        std::sort(doc_word_counts.begin(), doc_word_counts.end(),
                  [](const auto& a, const auto& b) {
                      return a.second > b.second; 
                  });

        for (const auto& [doc, word_count] : doc_word_counts) {
            double rank = global_max_word_count > 0 ? static_cast<double>(word_count) / global_max_word_count : 0;
            
            json doc_info = {
                {"document", doc},
                {"rank", std::round(rank * 1000.0) / 1000.0} 
            };
            query_result["results" + std::to_string(id)].push_back(doc_info);
        }

        if (!query_result["results" + std::to_string(id)].empty()) {
            query_result["result_found"] = true;
        }

        results.push_back(query_result);
        id++;
    }

    if (file.is_open()) {
        file << results.dump(4);
        std::cout << "Program answers successfully saved in answers.json" << std::endl;
    } else {
        std::cerr << "Error 404: Unable to open file." << std::endl;
    }
}



private:
    std::vector<std::string> files;
    std::string formula;
    int max_responses;
    FileIndexer indexer;
};