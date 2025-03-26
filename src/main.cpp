#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include <mutex>
#include "Search.h"
#include <iostream>
#include <future>

using json = nlohmann::json;
std::mutex mtx;
std::vector<std::string> extractQueries(const json& requests) {
    std::vector<std::future<std::string>> futures;
    for (const auto& request : requests["requests"]) {
        futures.push_back(std::async(std::launch::async, [&request]() {
            return request.get<std::string>();
        }));
    }
    std::vector<std::string> queries;
    for (auto& future : futures) {
        queries.push_back(future.get());
    }
    return queries;
}
int main() { 
    json requests; 
    json config_programm; 
    const std::string app_version = "0.9"; 
    try { 
        std::ifstream programm_name("../config.json"); 
        if (!programm_name.is_open()) { 
            throw std::runtime_error("Failed to open config.json."); 
        } 

        ConfigLoader config_loader("../config.json"); 
        SearchServer search_engine(config_loader); 
        
        std::ifstream file_request("../config/requests.json"); 
        if (!file_request.is_open()) { 
            throw std::runtime_error("Failed to open requests.json."); 
        } 

        file_request >> requests; 
        
        programm_name >> config_programm; 
        std::string version = config_programm["config"]["version"].get<std::string>(); 
        if (version != app_version) { 
            throw std::runtime_error("config.json has incorrect file version."); 
        } 
        std::cout << "Starting " << config_programm["config"]["name"].get<std::string>() << std::endl; 
        std::vector<std::string> queries = extractQueries(requests);
        search_engine.search(queries); 
        std::cout << "Program exit!" << std::endl; 
    } catch (const std::runtime_error& e) { 
        std::cerr << e.what() << std::endl; 
    } 
}