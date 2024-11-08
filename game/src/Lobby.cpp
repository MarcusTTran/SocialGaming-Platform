#include "Lobby.h"

using networking::Message;

Lobby::Lobby(const Game &game, networking::Server &server, std::shared_ptr<networking::Connection> lobbyCreator,
             std::string lobbyCode)
    : game(std::make_unique<Game>(game.getGameName())), server(server), lobbyCreator(lobbyCreator),
      lobbyCode(lobbyCode), state(LobbyState::Waiting) {
  server.send({{*lobbyCreator, "Lobby created with code: " + lobbyCode}});
}

Lobby::LobbyState Lobby::getState() { return state; }

void Lobby::addPlayer(const Player &player) {
  sendWelcomeMessage(player);

  players.push_back(player);
  sendCurrentListOfPlayers();
}

void Lobby::removePlayer(const Player &player) {
  sendToPlayer(player, "You have left the lobby.");
  players.erase(std::remove(players.begin(), players.end(), player), players.end());
  sendToAll(player.getDisplayName() + " has left the lobby.");
  sendCurrentListOfPlayers();
}

void Lobby::sendToAll(const std::string &message) {
  std::deque<networking::Message> outgoing;

  for (const auto &player : players) {
    outgoing.push_back({player.getConnection(), message});
  }

  server.send(outgoing);
}

void Lobby::sendToPlayer(const Player &player, const std::string &message) {
  server.send({{player.getConnection(), message}});
}

void Lobby::sendWelcomeMessage(const Player &player) {
  std::string welcomeMessage = "Welcome to the lobby, " + player.getDisplayName() + "!";
  welcomeMessage += " The game is " + game->getGameName();

  sendToPlayer(player, welcomeMessage);
}

void Lobby::sendCurrentListOfPlayers() {
  std::string message = "Current players in the lobby: ";
  for (const auto &player : players) {
    message += player.getDisplayName() + ", ";
  }
  server.send({{*lobbyCreator, message}});
}

void Lobby::addMessage(const Message &message) { incomingMessages.push_back(message); }

void Lobby::processIncomingMessage(const networking::Connection &connection, const std::string &message) {

  // check if the connection is the lobby creator
  if (connection.id == lobbyCreator->id) {
    if (message == "start") {
      sendToAll("Game starting!");
      state = LobbyState::InProgress;

      // This is where the game would start
    } else {
      std::string errorMessage = "Invalid command. Please type 'start' to start the game.";
      server.send({{connection, errorMessage}});
      return;
    }
  }

  auto player = std::find_if(players.begin(), players.end(),
                             [&](const Player &p) { return p.getConnection().id == connection.id; });

  // This is where we would process the meesage from the player if the game was running.
  // The game would be responsible for processing the message and sending the appropriate
  // messages back to the players. For now, we will just echo the message back to the player.
  // Untill we have a more concrete implementation of the game logic.
  if (player != players.end()) {
    if (state == LobbyState::InProgress) {
      // Add the incoming message to the queue of messages to be processed
      // Mesages will be processed on the next call to update()
      networking::Message incomingMessage = {connection, message};
      addMessage(incomingMessage);
    } else if (message == "leave") {
      removePlayer(*player);
    } else {
      sendToPlayer(*player, "You said: " + message);
    }
  }
}
