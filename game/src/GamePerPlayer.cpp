#include "GamePerPlayer.h"
#include "GameParser.h"

GamePerPlayer::GamePerPlayer(const ParsedGameData &parsedData) { perPlayerVariables = parsedData.getPerPlayer(); }

DataValue::OrderedMapType GamePerPlayer::getPlayerVariables() { return perPlayerVariables; }

void GamePerPlayer::addPerPlayerVariable(std::string key, DataValue value) {
    perPlayerVariables.emplace_back(key, value);
}

// Returns the DataValue object by const reference if it exists with the key provided.
// Otherwise returns an static const empty DataValue object.
const DataValue &GamePerPlayer::gamePerPlayerGetter(const std::string &key) const {
    auto perPlayerVarIter = std::find_if(perPlayerVariables.begin(), perPlayerVariables.end(),
                                         [&key](const auto &pair) { return pair.first == key; });
    if (perPlayerVarIter == perPlayerVariables.end()) {
        // throw std::out_of_range("Key '" + key + "' not found in variables.");
        static const DataValue emptyDataValue;
        return emptyDataValue;
    }
    return perPlayerVarIter->second;
}

// Returns the new value the user passed in if successful.
// Otherwise returns an static const empty DataValue object.
DataValue GamePerPlayer::gamePerPlayerSetter(const std::string &key, DataValue newValue) {
    auto perPlayerVarIter = std::find_if(perPlayerVariables.begin(), perPlayerVariables.end(),
                                         [&key](const auto &pair) { return pair.first == key; });
    if (perPlayerVarIter == perPlayerVariables.end()) {
        // throw std::out_of_range("Key '" + key + "' not found in variables.");
        static const DataValue emptyDataValue;
        return emptyDataValue;
    }
    perPlayerVarIter->second = newValue;
    return perPlayerVarIter->second;
}
