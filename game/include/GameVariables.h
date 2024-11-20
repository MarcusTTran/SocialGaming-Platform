#pragma once

#include "CommonVariantTypes.h"
#include "GameParser.h"
#include <algorithm>
#include <string>

class GameVariables {
public:
    GameVariables(const ParsedGameData &parserObject);
    ~GameVariables() = default;
    GameVariables() = default; // Delete later

    // Access variables by key
    DataValue::OrderedMapType getVariables() const;
    const DataValue &getVariable(const std::string &key) const;
    void setVariable(const std::string &key, DataValue content);

private:
    DataValue::OrderedMapType variables;
    auto findVariable(const std::string &key) const;
    auto findVariable(const std::string &key);
};
