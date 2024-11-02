#include "GamePerAudience.h"


using PairOfPairs = std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>;
using DataType = std::variant<std::string, std::vector<PairOfPairs>>;


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

GamePerAudience::GamePerAudience(const ParsedGameData& parsedData) {
    map<string, vector<pair<pair<string, string>, pair<string, string>>>> perAudienceParsed = parsedData.getPerAudience();
    for (const auto& [key, entries] : perAudienceParsed) {
        for(const auto& element : entries){
            auto pair1 = element.first;
            auto pair2 = element.second;
            perAudienceVariables[key] = pair<pair1.first, pair1.second>;
            perAudienceVariables[key] = pair<pair2.first, pair2.second>;
        }   
    }

}
GamePerAudience::~GamePerAudience() = default; 

// Postcondition: returns a string representation of the type used
// for the data portion of the perAudience map.If none is found, returns error 
// must be handled.
std::string GamePerAudience::getType(const std::string& key) {
    if (std::holds_alternative<std::string>(perAudienceVariables[key])) {
        return "string";
    } else if (std::holds_alternative<std::vector<PairOfPairs>>(perAudienceVariables[key])) {
        return "vector<pair< pair<string, string>, pair<string,string> >>";
    } else {
        return std::unexpected("error");
    }
}
