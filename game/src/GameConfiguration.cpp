#include "GameConfiguration.h"
#include <algorithm>
#include <utility>
#include <vector>  

std::optional<std::pair<int, int>> GameConfiguration::Setup::getRange() const {
    return range;
}

std::optional<DataValue::EnumDescriptionType> GameConfiguration::Setup::getChoices() const {
    return choices;
}

std::optional<DataValue::OrderedMapType> GameConfiguration::Setup::getDefault() const {
    return defaultValue;
}

GameConfiguration::GameConfiguration(const ParsedGameData& parserObject)
    : gameName(parserObject.getGameName()), 
      playerRange(parserObject.getPlayerRange()),
      audience(parserObject.hasAudience()) 
{

    const auto& setupData = parserObject.getSetup();  
    for (const auto& orderedMap : setupData) {
        setup.push_back(extractSetupFromOrderedMap(orderedMap));
    }
}


GameConfiguration::GameConfiguration() 
: gameName(""), playerRange(std::make_pair(0, 0)), audience(false) 
{
    setup = {};
}

GameConfiguration::Setup GameConfiguration::extractSetupFromOrderedMap(const DataValue::OrderedMapType& orderedMap) const {
    Setup setupEntry;

    for (const auto& [key, value] : orderedMap) {
        if (value.getType() == "ORDERED_MAP") {
            const auto& nestedMap = value.asOrderedMap();
            for (const auto& [nestedKey, nestedValue] : nestedMap) {
                if (nestedKey == "kind" && nestedValue.getType() == "STRING") {
                    setupEntry.kind = nestedValue.asString();
                } else if (nestedKey == "prompt" && nestedValue.getType() == "STRING") {
                    setupEntry.prompt = nestedValue.asString();
                } else if (nestedKey == "range" && nestedValue.getType() == "RANGE") {
                    setupEntry.range = nestedValue.asRange();
                } else if (nestedKey == "choice" && nestedValue.getType() == "ENUM_DESCRIPTION") {
                    setupEntry.choices = nestedValue.asEnumDescription();
                } else if (nestedKey == "default" && nestedValue.getType() == "ORDERED_MAP") {
                    setupEntry.defaultValue = nestedValue.asOrderedMap();
                }
            }
        }

        // Handle the case where the current key-value pair does not require recursion
        if (key == "kind" && value.getType() == "STRING") {
            setupEntry.kind = value.asString();
        } else if (key == "prompt" && value.getType() == "STRING") {
            setupEntry.prompt = value.asString();
        } else if (key == "range" && value.getType() == "RANGE") {
            setupEntry.range = value.asRange();
        } else if (key == "choice" && value.getType() == "ENUM_DESCRIPTION") {
            setupEntry.choices = value.asEnumDescription();
        } else if (key == "default" && value.getType() == "ORDERED_MAP") {
            setupEntry.defaultValue = value.asOrderedMap();
        }
    }

    return setupEntry;
}



GameName GameConfiguration::getGameName() const {
    return gameName;
}

std::pair<int, int> GameConfiguration::getPlayerRange() const {
    return playerRange;
}

bool GameConfiguration::hasAudience() const {
    return audience;
}

std::vector<GameConfiguration::Setup> GameConfiguration::getSetup() const {
    return setup;
}

GameConfiguration::Setup* GameConfiguration::findSetupByName(const std::string& key) {
    auto setupFinder = std::find_if(setup.begin(), setup.end(), [&key](const Setup& s) {
        return s.name == key;
    });

    if (setupFinder != setup.end()) {
        return &(*setupFinder);
    } else {
        return nullptr;
    }
}

void GameConfiguration::setKind(const std::string& key, const std::string& kindValue) {
    if (auto* setupPtr = findSetupByName(key)) {
        setupPtr->kind = kindValue;
    } else {
        throw std::runtime_error("Setup with key '" + key + "' not found.");
    }
}

void GameConfiguration::setPrompt(const std::string& key, const std::string& promptValue) {
    if (auto* setupPtr = findSetupByName(key)) {
        setupPtr->prompt = promptValue;
    } else {
        throw std::runtime_error("Setup with key '" + key + "' not found.");
    }
}

void GameConfiguration::setRange(const std::string& key, const std::pair<int, int>& rangeValue) {
    if (auto* setupPtr = findSetupByName(key)) {
        setupPtr->range = rangeValue;
    } else {
        throw std::runtime_error("Setup with key '" + key + "' not found.");
    }
}

void GameConfiguration::setChoices(const std::string& key, const DataValue::EnumDescriptionType& choicesValue) {
    if (auto* setupPtr = findSetupByName(key)) {
        setupPtr->choices = choicesValue;
    } else {
        throw std::runtime_error("Setup with key '" + key + "' not found.");
    }
}

void GameConfiguration::setDefaultValue(const std::string& key, const DataValue::OrderedMapType& defaultValue) {
    if (auto* setupPtr = findSetupByName(key)) {
        setupPtr->defaultValue = defaultValue;
    } else {
        throw std::runtime_error("Setup with key '" + key + "' not found.");
    }
}
