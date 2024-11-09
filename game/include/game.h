// author: kwa132, Mtt8
#pragma once

#include "GameConfiguration.h"
#include "GameConstants.h"
#include "GameParser.h"
#include "GamePerAudience.h"
#include "GamePerPlayer.h"
#include "GameRules.h"
#include "GameVariables.h"
#include <map>
#include <string>
#include <vector>

/*
  This is game class that it is an API to interacte with parsing data.
*/

class Game {
public:
  // Game(const ParsedGameData& parserObject, const std::string& gameName);
  // Game(const Game& other);  // copy constructor
  Game(const std::string &gameName);
  ~Game() = default;

  std::string getGameName() const;
  std::string getGameCode() const;

  // TODO: Implement these methods
  void startGame();
  void updateGame();
  // void setGameCode(const std::string &gameCode);
  // GameConfiguration getConfiguration();
  // GameConstants getConstants();
  // GameVariables getVariables();
  // GamePerPlayer getPerPlayer();
  // GamePerAudience getPerAudience();
private:
  // GameConfiguration configuration;
  // GameConstants constants;
  // GameVariables variables;
  // GamePerPlayer perPlayer;
  // GamePerAudience perAudience;
  std::string gameName;
  std::string gameCode;
};
