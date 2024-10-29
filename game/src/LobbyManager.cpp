#include "LobbyManager.h"
#include <algorithm>
#include <random>

void LobbyManager::createLobby(Game &game, const networking::Connection &lobbyCreator) {

  std::string lobbyCode = generateLobbyCode();
  lobbies.emplace(lobbyCode, std::make_unique<Lobby>(
                                 game, server, std::make_shared<networking::Connection>(lobbyCreator), lobbyCode));

  // Track that this connection is the creator of the lobby
  lobbyCreators[lobbyCreator.id] = lobbyCode;
}

std::string LobbyManager ::generateLobbyCode() {
  static constexpr std::string_view digits = "0123456789";
  static const int codeLength = 6;

  thread_local std::random_device rd;
  thread_local std::mt19937 gen(rd());

  std::string newCode;
  newCode.reserve(codeLength);

  do {
    newCode.clear();
    std::generate_n(std::back_inserter(newCode), codeLength,
                    [&]() { return digits[std::uniform_int_distribution<>(0, digits.size() - 1)(gen)]; });
  } while (lobbies.find(newCode) != lobbies.end()); // Check for uniqueness

  return newCode;
}

// This method is called when a player wants to join a lobby using a lobby code
// The player is prompted to enter a display name if the lobby is found
void LobbyManager::addPlayerToLobby(const std::string &lobbyCode, const networking::Connection &connection) {

  auto lobby = findLobbyByCode(lobbyCode);
  if (lobby) {

    // Track that this connection is awaiting a display name
    pendingDisplayNames[connection.id] = lobbyCode;

    server.send({{connection, "Please enter a display name:"}});

  } else {
    server.send({{connection, "Lobby not found. Please check the code and try again."}});
  }
}

// This method is called when a player has entered a display name and is ready to join the lobby
void LobbyManager::addPlayerToLobbyWithDisplayName(const networking::Connection &connection, const Player &player) {
  std::string lobbyCode = pendingDisplayNames[connection.id];
  auto lobby = findLobbyByCode(lobbyCode);
  if (lobby) {
    lobby->addPlayer(player);
    playersInLobbies[connection.id] = lobbyCode;
  }
}

Lobby *LobbyManager::findLobbyByCode(const std::string &lobbyCode) {
  auto it = lobbies.find(lobbyCode);
  if (it != lobbies.end()) {
    return it->second.get();
  }
  return nullptr;
}

bool LobbyManager::isAwaitingDisplayName(const networking::Connection &connection) const {
  return pendingDisplayNames.find(connection.id) != pendingDisplayNames.end();
}

void LobbyManager::removeFromPendingDisplayNames(const networking::Connection &connection) {
  pendingDisplayNames.erase(connection.id);
}

void LobbyManager::routeMessage(const networking::Connection &connection, const std::string &message) {

  // Check if the connection is a lobby creator
  auto lobbyCreator = lobbyCreators.find(connection.id);

  if (lobbyCreator != lobbyCreators.end()) {
    auto lobby = findLobbyByCode(lobbyCreator->second);
    if (lobby) {
      lobby->processIncomingMessage(connection, message);
      return;
    }
  }

  // Check if the connection is a player in a lobby
  auto playerInLobby = playersInLobbies.find(connection.id);

  if (playerInLobby != playersInLobbies.end()) {
    auto lobby = findLobbyByCode(playerInLobby->second);
    if (lobby) {
      lobby->processIncomingMessage(connection, message);
    }
  }
}
