#pragma once

#include "GameParser.h"
#include <algorithm>
#include <string>

class GameConstants {
public:
    GameConstants(const ParsedGameData& parserObject);
    GameConstants() = default; // Delete later
    ~GameConstants() = default;


    // Access constants by key
    DataValue::OrderedMapType getConstants() const;
    const DataValue &getConstant(const std::string &key) const;

private:
    DataValue::OrderedMapType constants;
};
