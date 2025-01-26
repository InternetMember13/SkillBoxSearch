#include <nlohmann/json.hpp>
using json = nlohmann::json;
class ConfigLoader {
public:
    ConfigLoader(const std::string& config_file) {
        load_config(config_file);
    }

    const std::vector<std::string>& get_files() const {
        return files;
    }

    const std::string& get_formula() const {
        return formula;
    }

    int get_max_responses() const {
        return max_responses;
    }

private:
    std::vector<std::string> files;
    std::string formula;
    int max_responses;

    void load_config(const std::string& config_file) {
        std::ifstream file(config_file);
        json config_json;
        file >> config_json;
        files = config_json["files"].get<std::vector<std::string>>();
        formula = config_json["formula"].get<std::string>();
        max_responses = config_json["config"]["max_responses"].get<int>();
    }
};


