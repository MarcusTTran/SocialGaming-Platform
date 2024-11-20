// author: kwa132, mtt8
#pragma once

#include "GameParser.h"
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

class GameName {
private:
    std::string name;

public:
    GameName(const std::string &name) : name(name) {}
    std::string getName() const { return name; }
};

class GameConfiguration {
public:
    struct Setup {
        std::string name;
        std::optional<std::string> kind;
        std::optional<std::string> prompt;
        std::optional<std::pair<int, int>> range;
        std::optional<DataValue::EnumDescriptionType> choices;
        std::optional<DataValue::OrderedMapType> defaultValue;

        std::optional<std::pair<int, int>> getRange() const;
        std::optional<DataValue::EnumDescriptionType> getChoices() const;
        std::optional<DataValue::OrderedMapType> getDefault() const;
    };

    // Constructor
    GameConfiguration(const ParsedGameData &parserObject);
    ~GameConfiguration() = default;

    // Getter methods
    GameName getGameName() const;
    std::pair<int, int> getPlayerRange() const;
    bool hasAudience() const;
    std::vector<Setup> getSetup() const;
    Setup *findSetupByName(const std::string &key);
    void setKind(const std::string &key, const std::string &kindValue);
    void setPrompt(const std::string &key, const std::string &promptValue);
    void setRange(const std::string &key, const std::pair<int, int> &rangeValue);
    void setChoices(const std::string &key, const DataValue::EnumDescriptionType &choicesValue);
    void setDefaultValue(const std::string &key, const DataValue::OrderedMapType &defaultValue);

private:
    GameName gameName;
    std::pair<int, int> playerRange;
    bool audience;
    std::vector<Setup> setup;
    // Helper function to extract Setup from OrderedMapType
    Setup extractSetupFromOrderedMap(const DataValue::OrderedMapType &orderedMap) const;
};