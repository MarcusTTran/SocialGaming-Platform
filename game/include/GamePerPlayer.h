#pragma once

#include <variant>
#include <unordered_map>
#include <string>
#include "CommonVariantTypes.h"
#include "GameParser.h"

// NOTE: DataType is defined in CommonVariantTypes.h

class GamePerPlayer {
    DataValue::OrderedMapType perPlayerVariables;
    
    public:
        GamePerPlayer(const ParsedGameData& parsedData);
        ~GamePerPlayer() = default;
        DataValue::OrderedMapType getPlayerVariables(); 
        void addPerPlayerVariable(std::string key, DataValue value); 
        const DataValue & gamePerPlayerGetter(const std::string&) const;     
        DataValue gamePerPlayerSetter(const std::string&, DataValue newValue);  // perPlayerVariables[key] = winner = 3; 
};
