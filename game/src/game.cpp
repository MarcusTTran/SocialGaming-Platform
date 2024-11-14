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

// Game::Game(const std::string &gameName, NameResolver &nameResolver)
//     : gameName(gameName), nameResolver(std::make_unique<NameResolver>(nameResolver)) {}

Game::Game(const std::string &gameName) : gameName(gameName) {}

std::string Game::getGameCode() const { return gameCode; }

std::string Game::getGameName() const { return gameName; }

void Game::startGame(const std::vector<Player> &players) {
  std::string key = "players";

  // Todo: Add the players to the name resolver
  // Need tp fix the types

  // nameResolver->add_new_value(key, players);
}

void Game::updateGame() {}

void Game::insertIncomingMessages(const std::deque<Message> &incomingMessages) {
  std::string key = "incomingMessages";
  std::unordered_map<std::string, std::deque<Message>> incomingMessagesMap;
  incomingMessagesMap[key] = incomingMessages;

  // TODO: Add the incoming messages to the name resolver
  // nameResolver->add_new_value(key, incomingMessagesMap);
}

// GameConfiguration Game::getConfiguration(){ return configuration; }
// GameConstants Game::getConstants() { return constants; }
// GameVariables Game::getVariables() { return variables; }
// GamePerPlayer Game::getPerPlayer() { return perPlayer; }
// GamePerAudience Game::getPerAudience() {return perAudience; }

// void Game::setGameCode(const std::string &gameCode) {
//   this->gameCode = gameCode;
// }
