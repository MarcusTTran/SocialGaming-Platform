#include "game.h"
#include <unordered_map>
#include <string>
#include <unordered_set>
#include <memory>


/*
    GameManger class is responsible for managing the games that are currently being played.
    It will keep track of the games that are currently being played and will allow for the addition and removal of games.
    It will also allow for the retrieval of an an active game.
    It stores an unordered_set of active game codes and an unordered_map of active games with their associated game codes.
*/
class GameManager {
    public:
        GameManager();
        void addGame(std::shared_ptr<Game>);
        void removeGame(std::shared_ptr<Game>);
        std::shared_ptr<Game> getGame(const std::string&) const;
        std::string getGameCode(std::shared_ptr<Game>) const;
    private:
        std::string generateGameCode();
        std::unordered_set<std::string> activeGameCodes;
        std::unordered_map<std::string, std::shared_ptr<Game>> activeGames;

};