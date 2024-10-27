#include "game.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

/*
    GameManger class is responsible for managing the games that are currently
   being played. It will keep track of the games that are currently being played
   and will allow for the addition and removal of games. It will also allow for
   the retrieval of an an active game. It stores an unordered_set of active game
   codes and an unordered_map of active games with their associated game codes.
*/
class GameManager {
public:
  GameManager();
  Game *createGame();
  void addGame(std::unique_ptr<Game> newGame);
  void removeGame(const Game &toRemove);
  Game *getGame(const std::string &) const;
  std::string getGameCode(const Game &game) const;
  std::string generateGameCode();

private:
  const std::array<std::string, 2> gamesFiles = {
      "../config/minimal.game", "../config/rockPaperScissors.game"};
  std::unordered_set<std::string> activeGameCodes;
  std::unordered_map<std::string, std::unique_ptr<Game>> activeGames;
};