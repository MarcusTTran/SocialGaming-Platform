// author: kwa132, Mtt8
#pragma once

#include "CommonVariantTypes.h"
#include "GameConfiguration.h"
#include "GameConstants.h"
#include "GameParser.h"
#include "GameVariables.h"
#include "NameResolver.h"
#include "Player.h"
#include <map>
#include <ranges>
#include <string>
#include <vector>

/*
  This game class holds a global_map which contains all of the variables our
  game will need to run, including constants, rules, fresh variables, etc. It is instantiated
  by first parsing the configuration file, then creating the API objects in this class, then
  pushing those API objeccts onto the global_map.

  Note: Rules are created in the parser as of now. The parser calls the constructor for the
  rules as we parse them.
*/

class Game {
public:
    // For compilation purposes (remove later)
    struct Message {
        std::string message;
        Player player;
    };

    Game(const ParsedGameData &parserObject, const std::string &gameName);
    Game(const std::string &gameName);
    ~Game() = default;

    std::string getGameName() const;
    std::string getGameCode() const;

    // TODO: Implement these methods
    void startGame(const DataValue &players);
    void updateGame();
    void insertIncomingMessages(const std::deque<Message> &incomingMessages);
    void addObjectToGlobalMap(const std::string &key, const DataValue &value, NameResolver &globalMap);

private:
    std::shared_ptr<NameResolver> globalMap;
    std::string gameName;
    std::string gameCode;

    // API objects
    GameConfiguration configuration;
    GameConstants constants;
    GameVariables variables;

    // TODO: add rules to the Game once it is parseable and instantiable
    std::vector<std::unique_ptr<Rule>> rules;
    // std::vector<Rule> rules;    UNCOMMENT THIS LINE AND STUFF IN THE CONSTRUCTOR TO MAKE RULES NOT USE POINTERS

    // void setGameCode(const std::string &gameCode);
};
