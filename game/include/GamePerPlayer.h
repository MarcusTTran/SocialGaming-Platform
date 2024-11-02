#pragma once


using PairOfPairs = std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>;
using DataType = std::variant<std::string, std::vector<PairOfPairs>>;
using std::string;

class GamePerPlayer {
    std::unordered_map<std::string, DataType> perPlayerVariables;
    
    public:
        GamePerPlayer(const ParsedGameData& parsedData);
        ~GamePerPlayer() = default;
        std::unordered_map<std::string, DataType> getPlayerVariables();
        DataType& addPerPlayerVariable(std::string key, T value); 
        DataType & gamePerPlayerGetter(const string&) const;     
        void gamePerPlayerSetter(const string&);  // perPlayerVariables[key] = winner = 3; 
};
