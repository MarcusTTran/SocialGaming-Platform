#include "Player.h"
#include "Server.h"
#include "game.h"
#include <deque>
#include <memory>

/*
 *  A Lobby is a collection of Players who are waiting to play a Game. The
 *  Lobby is responsible for managing the Players and the Game, and for
 *  sending messages to the Players.
 */
class Lobby {

public:
  // State of the lobby (e.g. waiting for players, in progress, finished)
  enum class LobbyState { Waiting, InProgress, Finished };

  Lobby(const Game &game, networking::Server &server, std::shared_ptr<networking::Connection> lobbyCreator,
        std::string lobbyCode);
  ~Lobby() = default;
  void addPlayer(const Player &player);
  void removePlayer(const Player &player);
  void sendToAll(const std::string &message);
  void sendToPlayer(const Player &player, const std::string &message);
  void sendWelcomeMessage(const Player &player);
  void processIncomingMessage(const networking::Connection &connection, const std::string &message);
  void addMessage(const networking::Message &message);
  LobbyState getState();

  // This method is called from main loop to update the game state with incoming messages
  // Calls game.update() and processes the messages
  void update();

private:
  void sendCurrentListOfPlayers();
  std::unique_ptr<Game> game;
  std::vector<Player> players;
  networking::Server &server;
  // A queue for messages to be processed on game.update()
  std::deque<networking::Message> incomingMessages;

  // The state of the lobby
  LobbyState state;

  // The user who created the lobby
  // This user has special privileges in the lobby (e.g. starting the game)
  // Can use this to send messages to the lobby creator
  std::shared_ptr<networking::Connection> lobbyCreator;
  std::string lobbyCode;
};
