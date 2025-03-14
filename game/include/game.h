// author: kwa132, Mtt8
#pragma once

#include "CommonVariantTypes.h"
#include "GameConfiguration.h"
#include "GameConstants.h"
#include "GameParser.h"
#include "GameVariables.h"
#include "NameResolver.h"
#include "Player.h"
//#include "GameConfigEdit.h"
#include <map>
#include <ranges>
#include <string>
#include <vector>

class GameCreators;
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

    Game(std::shared_ptr<ParsedGameData> parserObject, const std::string &gameName, GameConfiguration &,GameCreators *);
    Game(const std::string &gameName, std::shared_ptr<IServer> server);
    ~Game() = default;

    std::string getGameName() const;
    std::string getGameCode() const;

    void startGame(const DataValue &players);
    void updateGame();
    void insertIncomingMessages(const std::deque<networking::Message> &incomingMessages);
    void addObjectToGlobalMap(const std::string &key, const DataValue &value, NameResolver &globalMap);
    bool isGameDone() const { return isDone; }
    int maxPlayers() const { return configuration.getPlayerRange().second; }
    bool hasAudience() const { return configuration.hasAudience(); }
    DataValue::OrderedMapType getPerAudienceMap() const { return perAudienceMap; }
    DataValue::OrderedMapType getPerPlayerMap() const { return perPlayerMap; }

private:
    std::shared_ptr<NameResolver> globalMap;
    std::string gameName;
    std::string gameCode;
    bool isDone = false;

    // API objects
    GameConfiguration configuration;
    GameConstants constants;
    GameVariables variables;
    std::vector<std::unique_ptr<Rule>> rules;
    DataValue::OrderedMapType perPlayerMap;
    DataValue::OrderedMapType perAudienceMap;

    std::vector<std::unique_ptr<Rule>>::iterator currentRule;
};
