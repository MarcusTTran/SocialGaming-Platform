// author: kwa132, Mtt8
#ifndef GAME_H
#define GAME_H

#pragma once

#include "GameConfiguration.h"
#include "GameConstants.h"
#include "GamePerAudience.h"
#include "GameRules.h"
#include "GameVariables.h"
#include "Player.h"
#include <map>
#include <string>
#include <vector>

using namespace std;
// class Game{
//     public:
//         virtual ~Game() = default;
//         virtual string determineWinner() = 0;
//         virtual void processPlayerChoice(networking::Connection, const
//         string&, networking::Server&) = 0; virtual void resetPlayerChoices()
//         = 0; virtual string getGameName() const = 0; virtual void
//         sendingMessage(networking::Server&, networking::Connection, const
//         string&) const = 0;
// };

class Game {
  // GamePerPlayer perPlayer;
  // GamePerAudience perAudience;
  // GameConfiguration configuration;
  // GameConstants constants;
  // GameVariables variables;
  // GameRules rules;
  vector<Player> players;
  string gameName;

public:
  // Constructor (cannot be instantiated without all provided fields ie. no
  // default constructor allowed)
  // Game(GameConfiguration &configuration, GameConstants &constants,
  //      GameVariables &variables, GamePerAudience &perAudience, GameRules &rules,
  //      vector<Player> players)
  //     : configuration(configuration), constants(constants),
  //       variables(variables), perAudience(perAudience), rules(rules),
  //       players(players) {
  //   // TODO: Perhaps deep copy Players vector?
  // }
  Game(const string& gameName);
  ~Game() = default; // Default destructor

  // Getters for important game data
  // GameConfiguration &getConfiguration();
  // GameConstants &getConstants();
  // GameVariables &getVariables();
  // GameRules &getRules();
  string getGameName() const;
  const vector<Player> &getPlayers() const;
  void addPlayer(const Player& player);
  void sendingMessage(networking::Server& server, networking::Connection connection, const string& message) const;

  // TODO: I feel like we should move this code to a Manager class.
  // void sendingMessage(networking::Server&, networking::Connection, const
  // string&); void processPlayerChoice(networking::Connection, const string&,
  // networking::Server&); void resetPlayerChoices();
};

#endif
