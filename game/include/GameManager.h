#include "game.h"
#include <unordered_map>
#include <string>
#include <unordered_set>


/*
    GameManger class is responsible for managing the games that are currently being played.
    It will keep track of the games that are currently being played and will allow for the addition and removal of games.
    It will also allow for the retrieval of an an active game.
    It stores an unordered_set of active game codes and an unordered_map of active games with their associated game codes.
*/
class GameManager {
    public:
        GameManager();
        void addGame(Game*);
        void removeGame(Game*);
        Game* getGame(std::string_view) const;
    private:
        std::string_view generateGameCode();
        std::unordered_set<std::string_view> activeGameCodes;
        std::unordered_map<std::string_view, Game*> activeGames;      
};