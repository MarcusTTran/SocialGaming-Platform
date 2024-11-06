#include "GameParser.h"
#include "GamePerPlayer.h"


GamePerPlayer::GamePerPlayer(const ParsedGameData& parsedData) {
    perPlayerVariables = parsedData.getPerPlayer();
}

DataValue::OrderedMapType GamePerPlayer::getPlayerVariables() {
    return perPlayerVariables;
}

void GamePerPlayer::addPerPlayerVariable(std::string key, DataValue value) {
    perPlayerVariables.emplace_back(key, value);
}

const DataValue & GamePerPlayer::gamePerPlayerGetter(const std::string& key) const {
    auto perPlayerVarIter = std::find_if(perPlayerVariables.begin(), perPlayerVariables.end(), 
    [&key](const auto& pair) {
        return pair.first == key;
    });
    if (perPlayerVarIter == perPlayerVariables.end()) {
        throw std::out_of_range("Key '" + key + "' not found in variables.");
    }
    return perPlayerVarIter->second;
}

void GamePerPlayer::gamePerPlayerSetter(const std::string& key, DataValue newValue) {
    auto perPlayerVarIter = std::find_if(perPlayerVariables.begin(), perPlayerVariables.end(), 
    [&key](const auto& pair) {
        return pair.first == key;
    });
    if (perPlayerVarIter == perPlayerVariables.end()) {
        throw std::out_of_range("Key '" + key + "' not found in variables.");
    }
    perPlayerVarIter->second = newValue;
}




