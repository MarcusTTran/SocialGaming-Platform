#pragma once

#include "CommonVariantTypes.h"
#include "GameParser.h"
#include <expected>
#include <string>
#include <unordered_map>
#include <variant>

// NOTE: DataType is defined in CommonVariantTypes.h

class GamePerAudience {
    // Note: may add more types to this variant in the future

private:
    DataValue::OrderedMapType perAudienceVariables;

public:
    GamePerAudience(const ParsedGameData &parsedData);
    ~GamePerAudience() = default;
    DataValue::OrderedMapType getAudienceVariables();
    void addPerAudienceVariable(std::string key, DataValue value);
    const DataValue &getPerAudienceVariable(const std::string &) const;
    DataValue gamePerAudienceSetter(const std::string &, DataValue newValue);

    // std::expected<string, string> getType(const std::string& key);
    // TODO: Perhaps create concrete error types later?
};
