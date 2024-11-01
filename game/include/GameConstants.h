#pragma once

#include <string>
#include "GameParser.h"

class GameConstants {
public:
    GameConstants(const ParsedGameData& parserObject) : constants(parserObject.getConstants()) {}
    ~GameConstants() = default;

    // Access constants by key
    std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>& getConstant(const std::string& key) {
        return constants.at(key);
    }

private:
    std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>> constants;
};

