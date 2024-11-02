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

GamePerAudience::GamePerAudience(const ParsedGameData& parsedData) {
    // map<string, vector<pair<pair<string, string>, pair<string, string>>>> perAudienceParsed = parsedData.getPerAudience();
    

}

// Postcondition: returns a string representation of the type used
// for the data portion of the perAudience map.If none is found, returns error 
// must be handled.
std::expected<std::string, string> 
GamePerAudience::getType(const std::string& key) {
    if (std::holds_alternative<std::string>(perAudienceVariables[key])) {
        return "string";
    } else if (std::holds_alternative<std::vector<PairOfPairs>>(perAudienceVariables[key])) {
        return "vector<pair< pair<string, string>, pair<string,string> >>";
    } else {
        return std::unexpected("error");
    }
}
