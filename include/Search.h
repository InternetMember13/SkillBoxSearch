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
    int i = 1;  
    for (const auto& query : queries) {  
        std::vector<std::string> processed_queries = process_query(query);  
        if (processed_queries.empty()) {  
            std::cerr << "Error: No valid words in the query: " << query << std::endl;  
            continue;  
        }  
        std::set<std::string> relevant_docs; 
        auto it = indexer.get_index().find(processed_queries[0]);  
        if (it != indexer.get_index().end()) {  
            for (const auto& file : it->second) {  
                relevant_docs.insert(file);  
            }  
        }  
        std::vector<std::future<void>> futures;
    for (size_t j = 1; j < processed_queries.size(); ++j) {
        futures.push_back(std::async(std::launch::async, [&, j]() {
            auto it = indexer.get_index().find(processed_queries[j]);
            if (it != indexer.get_index().end()) {
                std::set<std::string> current_docs(it->second.begin(), it->second.end());
                std::set<std::string> intersection;
                for (const auto& doc : relevant_docs) {
                    if (current_docs.count(doc)) {
                        intersection.insert(doc);
                    }
                }
                std::lock_guard<std::mutex> lock(mutex_threads);
                relevant_docs = intersection;
            } else {
                std::lock_guard<std::mutex> lock(mutex_threads);
                relevant_docs.clear();
            }
        }));
    }
    for (auto& future : futures) {
        future.get();
    }
        json query_result = {  
            {"query", query},  
            {"results" + std::to_string(i), json::array()}
        };  
        std::vector<std::pair<std::string, int>> doc_relevances;
        for (const auto& doc : relevant_docs) {  
            if (query_result["results" + std::to_string(i)].size() < max_responses) {    
                WordCountResult word_count_result = count_word_frequencies(doc, processed_queries);  
                int relevance = 0;
                for (const auto& [word, count] : word_count_result.requested_word_frequencies) {  
                    if (word_count_result.total_word_count > 0) {  
                        query_result["relevance_of_doc(s)"][doc] += count;
                        relevance += count;
                    }  
                }  
                if (relevance > 0) {
                    doc_relevances.emplace_back(doc, relevance); 
                }
            }  
        }  
        std::sort(doc_relevances.begin(), doc_relevances.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
        });
        for (const auto& [doc, relevance] : doc_relevances) {

            query_result["results" + std::to_string(i)].push_back(doc);
        }
        i++;  
        results.push_back(query_result);  
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