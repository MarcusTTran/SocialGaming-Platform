#include "GamePerAudience.h"

/*
Idea of what perAudience:

perAudience = {
    wins : 0
    weapons: {
        tomatoes : 1
        eggs : 2
    }
    ...
}
*/

GamePerAudience::GamePerAudience(const ParsedGameData &parsedData) {
    perAudienceVariables = parsedData.getPerAudience();
}

DataValue::OrderedMapType GamePerAudience::getAudienceVariables() { return perAudienceVariables; }

// Note: might delete this if we don't need to add mor perAudience variables after
// initially creating the game
void GamePerAudience::addPerAudienceVariable(std::string key, DataValue value) {
    perAudienceVariables.emplace(key, value);
}

const DataValue &GamePerAudience::getPerAudienceVariable(const std::string &key) const {
    auto perAudienceVarIter = std::find_if(perAudienceVariables.begin(), perAudienceVariables.end(),
                                           [&key](const auto &pair) { return pair.first == key; });
    if (perAudienceVarIter == perAudienceVariables.end()) {
        // throw std::out_of_range("Key '" + key + "' not found in variables.");
        static const DataValue emptyDataValue;
        return emptyDataValue;
    }

    return perAudienceVarIter->second;
}

DataValue GamePerAudience::gamePerAudienceSetter(const std::string &key, DataValue newValue) {
    auto perAudienceVarIter = std::find_if(perAudienceVariables.begin(), perAudienceVariables.end(),
                                           [&key](const auto &pair) { return pair.first == key; });
    if (perAudienceVarIter == perAudienceVariables.end()) {
        // throw std::out_of_range("Key '" + key + "' not found in variables.");
        static const DataValue emptyDataValue;
        return emptyDataValue;
    }

    perAudienceVarIter->second = newValue;
    return perAudienceVarIter->second;
}

// // Postcondition: returns a string representation of the type used
// // for the data portion of the perAudience map.If none is found, returns error
// // must be handled.
// std::expected<std::string, string>
// GamePerAudience::getType(const std::string& key) {
//     if (std::holds_alternative<std::string>(perAudienceVariables[key])) {
//         return "string";
//     } else if (std::holds_alternative<std::vector<PairOfPairs>>(perAudienceVariables[key])) {
//         return "vector<pair< pair<string, string>, pair<string,string> >>";
//     } else {
//         return std::unexpected("error");
//     }
// }
