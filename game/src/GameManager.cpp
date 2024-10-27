#include "GameManager.h"

GameManager::GameManager() {
  activeGameCodes = std::unordered_set<std::string>();
  activeGames = std::unordered_map<std::string, std::unique_ptr<Game>>();
}

Game *GameManager::getGame(const std::string &gameCode) const {

  if (activeGames.find(gameCode) == activeGames.end()) {
    return nullptr;
  }
  return activeGames.at(gameCode).get();
}

std::string GameManager::getGameCode(const Game &game) const {
  for (const auto &it : activeGames) {
    if (it.second.get()->getGameName() == game.getGameName()) {
      return it.first;
    }
  }
  return "";
}

// Generates a random game game code
std::string GameManager::generateGameCode() {
  std::string gameCode = "0000";
  while (activeGameCodes.find(gameCode) != activeGameCodes.end()) {
    gameCode = std::to_string(rand() % 10000);
  }
  return gameCode;
}

void GameManager::addGame(std::unique_ptr<Game> newGame) {
  activeGameCodes.insert(newGame->getGameCode());
  activeGames[newGame.get()->getGameCode()] = std::move(newGame);
}

void GameManager::removeGame(const Game &game) {
  for (auto it = activeGames.begin(); it != activeGames.end(); it++) {
    if (it->second.get()->getGameCode() == game.getGameCode()) {
      activeGameCodes.erase(it->first);
      activeGames.erase(it);
      break;
    }
  }
}