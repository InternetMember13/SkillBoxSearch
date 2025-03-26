#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class ConfigLoader {
public:
    ConfigLoader(const std::string& config_file);

    const std::vector<std::string>& get_files() const;
    int get_max_responses() const;

private:
    std::vector<std::string> files;
    int max_responses;

    void load_config(const std::string& config_file);
};

#endif 
