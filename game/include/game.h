// author: kwa132, Mtt8

#pragma once

#include <string>
#include <map>
#include "Player.h"

using namespace std;

class Game{
    public:
        Game(string gameName) : gameName(gameName) {}
        ~Game();
        string determineWinner();
        void processPlayerChoice(networking::Connection, const string&, networking::Server&);
        void resetPlayerChoices(); 
        string getGameName() const;
        void sendingMessage(networking::Server&, networking::Connection, const string&) const;
        void addPlayer(Player);
        vector<Player> getPlayers() const;
    private:
        string gameName;
        vector<Player> players;
};