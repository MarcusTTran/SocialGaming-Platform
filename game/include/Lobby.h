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
  Lobby(const Game &game, networking::Server &server, std::shared_ptr<networking::Connection> lobbyCreator,
        std::string lobbyCode);
  ~Lobby() = default;
  void addPlayer(const Player &player);
  void sendToAll(const std::string &message);
  void sendToPlayer(const Player &player, const std::string &message);
  void sendWelcomeMessage(const Player &player);
  void processIncomingMessage(const networking::Connection &connection, const std::string &message);
  std::vector<Player> getPlayers() const;

private:
  void sendCurrentListOfPlayers();
  std::unique_ptr<Game> game;
  std::vector<Player> players;
  networking::Server &server;

  // The user who created the lobby
  // This user has special privileges in the lobby (e.g. starting the game)
  // Can use this to send messages to the lobby creator
  std::shared_ptr<networking::Connection> lobbyCreator;
  std::string lobbyCode;
};
