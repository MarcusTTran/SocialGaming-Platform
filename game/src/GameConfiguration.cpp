#include "GameConfiguration.h"
#include <algorithm>
#include <utility>

std::optional<std::pair<int, int>> GameConfiguration::Setup::getRange() const {
    auto findPair = std::find_if(value.begin(), value.end(), [](const auto& data) {
        return std::holds_alternative<std::pair<int, int>>(data);
    });
    
    if (findPair != value.end()) {
        return std::get<std::pair<int, int>>(*findPair);
    }
    return std::nullopt;
}


std::optional<std::vector<std::map<std::string, std::string>>> GameConfiguration::Setup::getDefault() const {
    auto findMap = std::find_if(defaultValue.begin(), defaultValue.end(), [](const auto& data) {
        return std::holds_alternative<std::map<std::string, std::string>>(data);
    });

    // Since `defaultValue` was defined to hold individual maps, we need to wrap it
    if (findMap != defaultValue.end()) {
        std::vector<std::map<std::string, std::string>> defaults;
        defaults.push_back(std::get<std::map<std::string, std::string>>(*findMap));
        return defaults;
    }
    return std::nullopt;
}


GameConfiguration::GameConfiguration(const std::string& fileContent)
    : config(ParsedGameData(fileContent)), gameName(config.getGameName()) {}

GameConfiguration::Setup GameConfiguration::extractSetupFromEntry(
    const std::string& key, const std::vector<std::map<std::string, std::string>>& value) {
    Setup setup;
    setup.name = key;

    for (const auto& ele : value) {
        for (const auto& [k, v] : ele) {
            if (k == "kind") {
                setup.kind = v;
            } else if (k == "prompt") {
                setup.prompt = v;
            } else if (k == "default") {
                setup.defaultValue.emplace_back(std::map<std::string, std::string>{{k, v}});
            } else {
                setup.value.emplace_back(std::map<std::string, std::string>{{k, v}});
            }
        }
    }
    return setup;
}

void GameConfiguration::processParsingData() {
    this->playerRange = config.getPlayerRange();
    this->audience = config.hasAudience();
    auto setups = config.getSetup();

    for (const auto& subSetup : setups) {
        for (const auto& [key, value] : subSetup) {
            setup.push_back(extractSetupFromEntry(key, value));
        }
    }
}

GameName GameConfiguration::getGameName() const {
    return gameName;
}

std::pair<int, int> GameConfiguration::getPlayerRange() const {
    return playerRange;
}

bool GameConfiguration::hasAudience() {
    return audience;
}

std::vector<GameConfiguration::Setup> GameConfiguration::getSetup() {
    return setup;
}

void GameConfiguration::setSetupName(Setup& setup, const std::string& name) {
    setup.name = name;
}

void GameConfiguration::setSetupKind(Setup& setup, const std::string& kind) {
    setup.kind = kind;
}

void GameConfiguration::setSetupPrompt(Setup& setup, const std::string& prompt) {
    setup.prompt = prompt;
}

void GameConfiguration::setSetupValue(Setup& setup, 
                                      std::vector<std::variant<std::map<std::string, std::string>, std::pair<int, int>>> value,
                                      std::vector<std::map<std::string, std::string>> defaultValue) {
    setup.value = std::move(value);

    // Wrap each default value map in a variant before assigning
    for (const auto& def : defaultValue) {
        setup.defaultValue.emplace_back(def);
    }
}
