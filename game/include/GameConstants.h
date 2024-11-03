#pragma once

#include <string>
#include <algorithm>
#include "GameParser.h"

class GameConstants {
public:
    GameConstants(const ParsedGameData& parserObject) : constants(parserObject.getConstants()) {}
    ~GameConstants() = default;

    // Access constants by key
    // vector<pair<string, DataValue>>
    DataValue& getConstant(const std::string& key) {
        auto valueFinder = std::find_if(begin(constants), end(constants), 
            [&key](const std::pair<std::string, DataValue>& entry){
                return entry.first == key;
            }
        );
        if(valueFinder != end(constants)){
            return valueFinder->second;
        }
        throw std::out_of_range("Key '" + key + "' not found in constants.");
    }

private:
    DataValue::OrderedMapType constants;
};

