#include "game.h"
#include <ranges>
#include <string_view>

// Calls the constructors for the API objects and initializes the game name
Game::Game(const ParsedGameData &parserObject, const std::string &gameName)
    : gameName(gameName), configuration(parserObject), constants(parserObject), variables(parserObject),
      globalMap(std::make_shared<NameResolver>()) {

    if (!globalMap) {

        std::cerr << "Error: globalMap is not initialized" << std::endl;
    }

    // TODO: perhaps add rules? {
    // Populate the global map with other API variables held in Game object
    addObjectToGlobalMap("constants", DataValue(constants.getConstants()), *globalMap);
    addObjectToGlobalMap("variables", DataValue(variables.getVariables()), *globalMap);

    // add configuration to global map
    DataValue::OrderedMapType configurationMap;
    configurationMap.push_back(std::make_pair(std::string("name"), DataValue(gameName)));
    configurationMap.push_back(std::make_pair(std::string("player range"), DataValue(configuration.getPlayerRange())));
    configurationMap.push_back(std::make_pair(std::string("audience"), DataValue(configuration.hasAudience())));

    // Convert setup rules to a vector of DataValue TODO: fix this by testing and making better with ranges
    std::vector<DataValue> setupData;
    for (const auto &setup : parserObject.getSetup()) {
        DataValue::OrderedMapType setupRuleMap;
        for (const auto &[key, value] : setup) {
            setupRuleMap.push_back(std::make_pair(key, value));
        }
        setupData.push_back(DataValue(setupRuleMap));
    }
    configurationMap.push_back(std::make_pair("setup", DataValue(setupData)));
    // configurationMap.push_back(std::make_pair(std::string("setup"), DataValue(setupFromParser)) );
    addObjectToGlobalMap(std::string("configuration"), DataValue(configurationMap), *globalMap);
}

Game::Game(const std::string &gameName) : gameName(gameName), globalMap(std::make_shared<NameResolver>()) {}

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

void Game::startGame(DataValue &players) {
    std::string key = "players";
    std::vector<DataValue> playersVector = players.asList();

    std::cout << "Players: " << playersVector.size() << std::endl;
    globalMap->addNewValue(key, DataValue(playersVector));

    // for testing purposes only
    auto playersMap = globalMap->getValue(key);
    auto playersList = playersMap.asList();

    // for testing purposes
    // print out all the players

    for (const auto player : playersList) {

        std::cout << player.asOrderedMap().at(0).second.asNumber() << std::endl;
        std::cout << player.asOrderedMap().at(1).second.asString() << std::endl;
    }
}

void Game::insertIncomingMessages(const std::deque<Message> &incomingMessages) {
    std::string key = "incomingMessages";
    std::unordered_map<std::string, std::deque<Message>> incomingMessagesMap;
    incomingMessagesMap[key] = incomingMessages;

    // TODO: Add the incoming messages to the name resolver
    // nameResolver->add_new_value(key, incomingMessagesMap);
}
