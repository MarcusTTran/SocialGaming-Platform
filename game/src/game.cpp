#include "game.h"

Game::Game(const std::string& name) : gameName(name){}

void Game::addPlayer(const Player& player) {
    players.push_back(player);
}

std::string Game::getGameName() const {
    return gameName;
}

const std::vector<Player>& Game::getPlayers() const {
    return players;
}

void Game::sendingMessage(networking::Server& server, networking::Connection connection, const std::string& message) const {
    server.send({{connection, message}});
}

