#pragma once

#include <variant>
#include <unordered_map>
#include <string>
#include "CommonVariantTypes.h"
#include "GameParser.h"

// NOTE: DataType is defined in CommonVariantTypes.h

class GamePerPlayer {
    std::unordered_map<std::string, std::vector<DataType> > perPlayerVariables;
    
    public:
        GamePerPlayer(const ParsedGameData& parsedData);
        ~GamePerPlayer() = default;
        std::unordered_map<std::string, DataType> getPlayerVariables();
        DataType& addPerPlayerVariable(std::string key, DataType value); 
        DataType & gamePerPlayerGetter(const std::string&) const;     
        void gamePerPlayerSetter(const std::string&);  // perPlayerVariables[key] = winner = 3; 
};
