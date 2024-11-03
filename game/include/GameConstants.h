#pragma once

#include <string>
#include <algorithm>
#include "GameParser.h"

class GameConstants {
public:
    GameConstants(const ParsedGameData& parserObject);
    ~GameConstants() = default;

    // Access constants by key
    // vector<pair<string, DataValue>>
    const DataValue& getConstant(const std::string& key)const;

private:
    DataValue::OrderedMapType constants;
};

