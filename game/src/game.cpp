#include "game.h"
#include <ranges>
#include <string_view>

// Calls the constructors for the API objects and initializes the game name
Game::Game(ParsedGameData &parserObject, const std::string &gameName)
    : gameName(gameName), configuration(parserObject), constants(parserObject), variables(parserObject),
      globalMap(std::make_shared<NameResolver>()) {

    rules = parserObject.moveRules();
    std::cout << "Rules size: " << rules.size() << std::endl;

    // Populate the global map with other API variables held in Game object
    addObjectToGlobalMap("constants", DataValue(constants.getConstants()), *globalMap);
    addObjectToGlobalMap("variables", DataValue(variables.getVariables()), *globalMap);
    // addObjectToGlobalMap("rules", DataValue(rules), *globalMap);

    // Add configuration to global map
    DataValue::OrderedMapType configurationMap;
    configurationMap.emplace("name", DataValue(gameName));
    configurationMap.emplace("player range", DataValue(configuration.getPlayerRange()));
    configurationMap.emplace("audience", DataValue(configuration.hasAudience()));

    // Convert setup rules to a vector of DataValue
    std::vector<DataValue> setupData;
    for (const auto &setup : parserObject.getSetup()) {
        DataValue::OrderedMapType setupRuleMap;
        for (const auto &[key, value] : setup) {
            setupRuleMap.emplace(key, value);
        }
        setupData.push_back(DataValue(setupRuleMap));
    }
    configurationMap.emplace("setup", DataValue(setupData));

    addObjectToGlobalMap("configuration", DataValue(configurationMap), *globalMap);

    currentRule = rules.begin();
}

// Game::Game(const std::string &gameName, NameResolver &nameResolver)
//     : gameName(gameName), nameResolver(std::make_unique<NameResolver>(nameResolver)) {}

Game::Game(const std::string &gameName, std::shared_ptr<IServer> server)
    : gameName(gameName), globalMap(std::make_shared<NameResolver>()) {

    // Add a simple input rule to the game
    std::unique_ptr<Rule> rule = std::make_unique<SimpleInputRule>(server);
    std::unique_ptr<AllPlayersRule> allPlayersRule = std::make_unique<AllPlayersRule>();
    std::unique_ptr<StringRule> simpleStringRule = std::make_unique<StringRule>("Hello, World!");
    std::unique_ptr<Rule> rule2 =
        std::make_unique<MessageRule>(server, std::move(allPlayersRule), std::move(simpleStringRule));
    rules.push_back(std::move(rule));
    rules.push_back(std::move(rule2));
    currentRule = rules.begin();
}

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

void Game::startGame(const DataValue &players) {
    std::string key = "players";
    auto playersMap = players.asOrderedMap();

    globalMap->addNewValue(key, players);

    auto playersMapValue = globalMap->getValue(key).value().asOrderedMap();

    for (const auto &[playerName, playerData] : playersMapValue) {
        auto player = playerData.asOrderedMap();
        auto name = player["name"].asString();
        std::cout << "Player name: " << name << std::endl;
    }

    std::cout << "Game started." << std::endl;
    // Start running rules here

    while (currentRule != rules.end()) {
        auto returnValue = (*currentRule)->runBurst(*globalMap);

        if (returnValue.isCompleted()) {
            currentRule++;
        } else {
            break;
        }
    }

    if (currentRule == rules.end()) {
        std::cout << "All rules completed." << std::endl;
    }
}

// Inserts incoming messages into the the NameResolver
void Game::insertIncomingMessages(const std::deque<networking::Message> &incomingMessages) {

    if (incomingMessages.empty()) {
        return;
    }

    std::string key = "incoming_messages";
    DataValue::OrderedMapType incomingMessagesMap;

    for (const auto &message : incomingMessages) {
        std::string messageKey = std::to_string(message.connection.id);
        incomingMessagesMap.emplace(messageKey, DataValue(message.text));
    }

    DataValue incomingMessagesValue(incomingMessagesMap);
    globalMap->addNewValue(key, incomingMessagesValue);

    std::cout << "Incoming messages inserted into global map." << std::endl;

    for (const auto &[key, value] : incomingMessagesMap) {
        std::cout << "Key: " << key << " Value: " << value.asString() << std::endl;
    }
}

void Game::updateGame() {

    // start off from the last rules that was not completed

    while (currentRule != rules.end()) {
        auto returnValue = (*currentRule)->runBurst(*globalMap);

        if (returnValue.isCompleted()) {
            currentRule++;
        } else {
            break;
        }
    }

    if (currentRule == rules.end()) {
        std::cout << "All rules completed." << std::endl;
    }
}