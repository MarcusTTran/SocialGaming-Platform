#include "game.h"

Game::Game(const std::string &gameName) : gameName(gameName) {}
void Game::addPlayer(const Player &player) { players.push_back(player); }
std::string Game::getGameCode() const { return gameCode; }

std::string Game::getGameName() const { return gameName; }

const std::vector<Player> &Game::getPlayers() const { return players; }

void Game::sendingMessage(networking::Server &server, const Player &player,
                          const std::string &message) const {
  server.send({{player.getConnection(), message}});
}

void Game::setGameCode(const std::string &gameCode) {
  this->gameCode = gameCode;
}
