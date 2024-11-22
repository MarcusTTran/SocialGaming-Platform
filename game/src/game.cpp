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
    
    // Add configuration to global map
    DataValue::OrderedMapType configurationMap;
    configurationMap.emplace("name", DataValue(gameName));
    configurationMap.emplace("player range", DataValue(configuration.getPlayerRange()));
    configurationMap.emplace("audience", DataValue(configuration.hasAudience()));

    // Convert setup rules to a vector of DataValue
    std::vector<DataValue> setupData;
    for (const auto& setup : parserObject.getSetup()) {
        DataValue::OrderedMapType setupRuleMap;
        for (const auto& [key, value] : setup) {
            setupRuleMap.emplace(key, value);
        }
        setupData.push_back(DataValue(setupRuleMap));
    }
    configurationMap.emplace("setup", DataValue(setupData));

    addObjectToGlobalMap("configuration", DataValue(configurationMap), globalMap);
}

// Game::Game(const std::string &gameName, NameResolver &nameResolver)
//     : gameName(gameName), nameResolver(std::make_unique<NameResolver>(nameResolver)) {}

Game::Game(const std::string &gameName) : gameName(gameName) {}

std::string Game::getGameName() const { return gameName; }

// Adds an object to the global map provided a key and a DataValue object
void Game::addObjectToGlobalMap(const std::string &key, const DataValue &value, NameResolver &globalMap) {
    bool operationResult = globalMap.addNewValue(key, value);
    if (!operationResult) {
        std::string_view errorMsg = "Error: key already exists in the global map.\n";
        std::string errorDef = "Tried to add key: {" + key + " }";
        std::cerr << std::string(errorMsg) << errorDef << std::endl;
    }
}

void Game::startGame(const std::vector<Player> &players) {
    std::string key = "players";

    // Todo: Add the players to the name resolver
    // Need tp fix the types

    // nameResolver->add_new_value(key, players);
}

void Game::insertIncomingMessages(const std::deque<Message> &incomingMessages) {
    std::string key = "incomingMessages";
    std::unordered_map<std::string, std::deque<Message>> incomingMessagesMap;
    incomingMessagesMap[key] = incomingMessages;

    // TODO: Add the incoming messages to the name resolver
    // nameResolver->add_new_value(key, incomingMessagesMap);
}
