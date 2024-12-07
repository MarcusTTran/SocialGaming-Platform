#include "game.h"
#include <ranges>
#include <string_view>

// Calls the constructors for the API objects and initializes the game name
Game::Game(std::shared_ptr<ParsedGameData> parserObject, const std::string &gameName, GameConfiguration &gameConfig)
    : gameName(gameName), configuration(gameConfig), constants(*parserObject), variables(*parserObject),
      globalMap(std::make_shared<NameResolver>()), perPlayerMap(parserObject->getPerPlayer()),
      perAudienceMap(parserObject->getPerAudience()) {

    rules = parserObject->moveRules();
    std::cout << "Rules size: " << rules.size() << std::endl;

    // Populate the global map with other API variables held in Game object
    globalMap->addInnerScope();
    addObjectToGlobalMap("constants", DataValue(constants.getConstants()), *globalMap);

    auto constants = globalMap->getValue("constants").value().asOrderedMap();

    for (const auto &[key, value] : constants) {
        std::cout << "Key: " << key << " Value: " << value << std::endl;
    }

    addObjectToGlobalMap("variables", DataValue(variables.getVariables()), *globalMap);
    // addObjectToGlobalMap("rules", DataValue(rules), *globalMap);

    // Add configuration to global map
    DataValue::OrderedMapType configurationMap;
    configurationMap.emplace("name", DataValue(gameName));
    configurationMap.emplace("player range", DataValue(configuration.getPlayerRange()));
    configurationMap.emplace("audience", DataValue(configuration.hasAudience()));

    // NOTE: getSetup() for some reason always contains 1 empty setup at the end. Keep that in mind when accessing
    // things.
    auto &setups = configuration.getSetup(); // configuration doesn't work, not sure why. So i'm passing in the edited
                                             // game config object aswell so it works.

    for (auto i : setups) {
        std::cout << "Config Setup Names : " << i.name << '\n';
    }

    configurationMap.emplace("rounds", DataValue(setups.at(0).round));

    // Convert setup rules to a vector of DataValue
    std::vector<DataValue> setupData;
    std::cout << "Setup size: " << configuration.getSetup().size() << std::endl;
    std::cout << "Setup: " << std::endl;
    for (const auto &setup : parserObject->getSetup()) {
        DataValue::OrderedMapType setupRuleMap;
        for (const auto &[key, value] : setup) {
            std::cout << "key" << key << " value" << value << std::endl;
            setupRuleMap.emplace(key, value);
        }
        setupData.push_back(DataValue(setupRuleMap));
    }
    configurationMap.emplace("setup", DataValue(setupData));

    addObjectToGlobalMap("configuration", DataValue(configurationMap), *globalMap);

    auto setup = globalMap->getValue("configuration").value().asOrderedMap();

    std::cout << "Configuration map" << std::endl;
    for (const auto &[key, value] : setup) {
        std::cout << "Key: " << key << " Value: " << value << std::endl;
    }

    currentRule = rules.begin();
}

// Game::Game(const std::string &gameName, NameResolver &nameResolver)
//     : gameName(gameName), nameResolver(std::make_unique<NameResolver>(nameResolver)) {}

Game::Game(const std::string &gameName, std::shared_ptr<IServer> server)
    : gameName(gameName), globalMap(std::make_shared<NameResolver>()) {

    // // Add a simple input rule to the game
    // std::unique_ptr<Rule> rule = std::make_unique<SimpleInputRule>(server);
    // std::unique_ptr<AllPlayersRule> allPlayersRule = std::make_unique<AllPlayersRule>();
    // std::unique_ptr<StringRule> simpleStringRule = std::make_unique<StringRule>("Hello, World!");
    // std::unique_ptr<Rule> rule2 =
    //     std::make_unique<MessageRule>(server, std::move(allPlayersRule), std::move(simpleStringRule));
    // rules.push_back(std::move(rule));
    // rules.push_back(std::move(rule2));
    // currentRule = rules.begin();
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

    globalMap->addNewValue(key, players);

    auto playersList = globalMap->getValue(key).value().asList();

    for (const auto &player : playersList) {
        auto playerMap = player.asOrderedMap();
        std::cout << "Player: " << playerMap["name"].asString() << playerMap["__id__"].asNumber() << std::endl;
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

    DataValue::OrderedMapType incomingMessagesMap;

    std::string key = "incoming_messages";

    for (const auto &message : incomingMessages) {
        std::string messageKey = std::to_string(message.connection.id);
        incomingMessagesMap.emplace(messageKey, DataValue(message.text));
    }

    DataValue incomingMessagesValue(incomingMessagesMap);
    globalMap->setValue(key, incomingMessagesValue);
    std::cout << "Incoming messages updated in global map" << std::endl;

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
        isDone = true;
    }
}