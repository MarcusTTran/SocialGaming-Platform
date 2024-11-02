#pragma once

#include <unordered_map>
#include <string>
#include <variant>
#include <expected>

using PairOfPairs = std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>;
using DataType = std::variant<std::string, std::vector<PairOfPairs>>;
using std::string;

class GamePerAudience {
// Note: may add more types to this variant in the future

private:
    std::unordered_map<string, DataType> perAudienceVariables;

public:
    GamePerAudience(const ParsedGameData& parsedData); 
    ~GamePerAudience() = default;
    std::unordered_map<string, DataType> getAudienceVariables();
    DataType& addPerAudienceVariable(string key, DataType value);
    DataType& getPerAudienceVariable(const string&) const;
    void gamePerAudienceSetter(const string&);

    std::expected<string, string> getType(const string& key);
    // TODO: Perhaps create concrete error types later? 
};
