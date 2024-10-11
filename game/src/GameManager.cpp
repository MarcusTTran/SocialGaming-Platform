#include "GameManager.h"


GameManager::GameManager() {
    activeGameCodes = std::unordered_set<std::string_view>();
    activeGames = std::unordered_map<std::string_view, Game*>();
}

Game* GameManager::getGame(std::string_view gameCode) const{
    return activeGames.at(gameCode);
}

// Generates a random game game code
std::string_view GameManager::generateGameCode(){
    std::string_view gameCode = "0000";
    while(activeGameCodes.find(gameCode) != activeGameCodes.end()){
        gameCode = std::to_string(rand() % 10000);
    }
    return gameCode;
    
}

void GameManager::addGame(Game* game){
    std::string_view gameCode = generateGameCode();
    activeGameCodes.insert(gameCode);
    activeGames[gameCode] = game;
}

void GameManager::removeGame(Game* game){
    for(auto it = activeGames.begin(); it != activeGames.end(); it++){
        if(it->second == game){
            activeGameCodes.erase(it->first);
            activeGames.erase(it);
            break;
        }
    }
}

