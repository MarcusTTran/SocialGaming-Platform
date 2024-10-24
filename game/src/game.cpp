#include "game.h"

Game::Game(const string& name) : gameName(name){}

void Game::addPlayer(const Player& player) {
    players.push_back(player);
}

string Game::getGameName() const {
    return gameName;
}

const vector<Player>& Game::getPlayers() const {
    return players;
}

void Game::sendingMessage(networking::Server& server, networking::Connection connection, const string& message) const {
    server.send({{connection, message}});
}

