#include "game.h"
#include <string_view>
#include <ranges>

// Calls the constructors for the API objects and initializes the game name
Game::Game(const ParsedGameData& parserObject, const std::string& gameName)
    : gameName(gameName),
      configuration(parserObject),
      constants(parserObject),
      variables(parserObject),
      // TODO: perhaps add rules?
      globalMap()
{ 
    // Populate the global map with other API variables held in Game object
    addObjectToGlobalMap("constants", DataValue(constants.getConstants()), globalMap);
    addObjectToGlobalMap("variables", DataValue(variables.getVariables()), globalMap);
    
    // add configuration to global map
    DataValue::OrderedMapType configurationMap;
    configurationMap.push_back(std::make_pair(std::string("name"), DataValue(gameName)) );
    configurationMap.push_back(std::make_pair(std::string("player range"), DataValue(configuration.getPlayerRange())) );
    configurationMap.push_back(std::make_pair(std::string("audience"), DataValue(configuration.hasAudience()) ) );

    // Convert setup rules to a vector of DataValue TODO: fix this by testing and making better with ranges
    std::vector<DataValue> setupData;
    for (const auto& setup : parserObject.getSetup()) {
        DataValue::OrderedMapType setupRuleMap;
        for (const auto& [key, value] : setup) {
            setupRuleMap.push_back(std::make_pair(key, value));
        }
        setupData.push_back(DataValue(setupRuleMap));
    }
    configurationMap.push_back(std::make_pair("setup", DataValue(setupData)));
    // configurationMap.push_back(std::make_pair(std::string("setup"), DataValue(setupFromParser)) );
    addObjectToGlobalMap(std::string("configuration"), DataValue(configurationMap), globalMap);
}

Game::Game(const std::string& gameName) : gameName(gameName) {}

std::string Game::getGameCode() const { return gameCode; }

std::string Game::getGameName() const { return gameName; }

// Adds an object to the global map provided a key and a DataValue object
void Game::addObjectToGlobalMap(const std::string &key, const DataValue &value, NameResolver &globalMap) {
    bool operationResult =  globalMap.addNewValue(key, value);
    if (!operationResult) {
        std::string_view errorMsg = "Error: key already exists in the global map.\n";
        std::string errorDef = "Tried to add key: {" + key + " }";
        
        std::cerr << std::string(errorMsg) << errorDef << std::endl;
    }
}



// GameConfiguration Game::getConfiguration(){ return configuration; }
// GameConstants Game::getConstants() { return constants; }
// GameVariables Game::getVariables() { return variables; }
// GamePerPlayer Game::getPerPlayer() { return perPlayer; }
// GamePerAudience Game::getPerAudience() {return perAudience; }

// void Game::setGameCode(const std::string &gameCode) {
//   this->gameCode = gameCode;
// }
