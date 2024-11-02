#pragma once

#include <unordered_map>
#include <string>
#include <variant>
#include <expected>
#include "CommonVariantTypes.h"
#include "GameParser.h"

// NOTE: DataType is defined in CommonVariantTypes.h

class GamePerAudience {
// Note: may add more types to this variant in the future

private:
    std::unordered_map<std::string, DataType> perAudienceVariables;

public:
    GamePerAudience(const ParsedGameData& parsedData); 
    ~GamePerAudience() = default;
    std::unordered_map<std::string, DataType> getAudienceVariables();
    DataType& addPerAudienceVariable(std::string key, DataType value);
    DataType& getPerAudienceVariable(const std::string&) const;
    void gamePerAudienceSetter(const std::string&);

    std::expected<string, string> getType(const std::string& key);
    // TODO: Perhaps create concrete error types later? 
};
