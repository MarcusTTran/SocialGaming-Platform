// author: kwa132, mtt8
#pragma once

#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include <variant>
#include <ranges>
#include "GameParser.h"


// TODO: need to fix all logic for future API
using NestedData = std::vector<std::map<std::string, std::vector<std::map<std::string, std::string>>>>;

class GameName {
private:
  const std::string &name;

public:
  // We will not allow an instance to be create without an input string
  GameName(const std::string &name) : name(name) {}
};

// Helper function to simplify access to variant data
template <typename T>
std::optional<T> getValue(const std::variant<std::map<std::string, std::string>, std::pair<int, int>>& data) {
    if (std::holds_alternative<T>(data)) {
        return std::get<T>(data);
    }
    return std::nullopt;
}

class GameConfiguration {
  public:
    /*
      Default:
        expression: $ => choice(
        $.boolean,
        $.number,
        $.quoted_string,
        $.list_literal,
        $.identifier,
        $.value_map,
    */
   
    struct Setup {
        std::string name;
        std::string kind;
        std::string prompt;
        
        // Ensure this variant matches the one used in the function
        std::vector<std::variant<std::map<std::string, std::string>, std::pair<int, int>>> value;
        std::vector<std::variant<std::map<std::string, std::string>>> defaultValue;

        std::optional<std::pair<int, int>> getRange() const;
        std::optional<std::vector<std::map<std::string, std::string>>> getDefault() const;
    };
    // constructor and destructor
    GameConfiguration(const std::string& fileContent); // const ParsedGameData& parsedData
    ~GameConfiguration() = default;
    // Getter and setters
    GameName getGameName() const;
    std::pair<int, int> getPlayerRange() const;
    bool hasAudience();
    std::vector<Setup> getSetup();
    void processParsingData();
    Setup extractSetupFromEntry(const std::string& key, const std::vector<std::map<std::string, std::string>>& value);
    void setSetupName(Setup& setup, const std::string& name);
    void setSetupKind(Setup& setup,const std::string& kind);
    void setSetupPrompt(Setup& setup,const std::string& prompt);
    void setSetupValue(Setup& setup,
                       std::vector<std::variant<std::map<std::string, std::string>, std::pair<int, int>>> value,
                       std::vector<std::map<std::string, std::string>> defaultValue = {});
  private: 
    ParsedGameData config; // Was thinking of deleting this
    const GameName gameName;
    std::pair<int, int> playerRange;
    bool audience;
    std::vector<Setup> setup;
};
