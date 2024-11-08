#include "game.h"

// Game::Game(const ParsedGameData& parserObject, const std::string& gameName)
//     : gameName(gameName),
//       configuration(parserObject),
//       constants(parserObject),
//       variables(parserObject),
//       perPlayer(parserObject),
//       perAudience(parserObject) 
// { }

// Game::Game(const Game& other)
//     : gameName(other.gameName),
//       gameCode(other.gameCode),
//       configuration(other.configuration),
//       constants(other.constants),
//       variables(other.variables),
//       perPlayer(other.perPlayer),
//       perAudience(other.perAudience) { }

Game::Game(const std::string& gameName) : gameName(gameName) {}

std::string Game::getGameCode() const { return gameCode; }

std::string Game::getGameName() const { return gameName; }

// GameConfiguration Game::getConfiguration(){ return configuration; }
// GameConstants Game::getConstants() { return constants; }
// GameVariables Game::getVariables() { return variables; }
// GamePerPlayer Game::getPerPlayer() { return perPlayer; }
// GamePerAudience Game::getPerAudience() {return perAudience; }

// void Game::setGameCode(const std::string &gameCode) {
//   this->gameCode = gameCode;
// }
