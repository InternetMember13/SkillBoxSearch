#include "ConfigLoader.h"

using json = nlohmann::json;

ConfigLoader::ConfigLoader(const std::string& config_file) {
    load_config(config_file);
}

const std::vector<std::string>& ConfigLoader::get_files() const {
    return files;
}

const std::string& ConfigLoader::get_formula() const {
    return formula;
}

int ConfigLoader::get_max_responses() const {
    return max_responses;
}

void ConfigLoader::load_config(const std::string& config_file) {
    std::ifstream file(config_file);
    json config_json;
    file >> config_json;
    files = config_json["files"].get<std::vector<std::string>>();
    formula = config_json["formula"].get<std::string>();
    max_responses = config_json["config"]["max_responses"].get<int>();
}
