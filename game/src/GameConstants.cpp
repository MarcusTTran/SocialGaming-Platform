#include "GameConstants.h"

GameConstants::GameConstants(const ParsedGameData& parserObject) : 
    constants(parserObject.getConstants()) {}

const DataValue& GameConstants::getConstant(const std::string& key) const{
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
