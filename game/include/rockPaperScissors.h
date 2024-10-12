// author: kwa132, Mtt8

#pragma once

#include "game.h"
#include "GameParser.h"
#include "algorithm"
#include "iostream"
#include <queue>

using namespace std;

struct ConnectionComparator {
    bool operator()(const networking::Connection& lhs, const networking::Connection& rhs) const {
        return lhs.id < rhs.id;  
    }
};

class RockPaperScissorsGame : public Game{
    public:
        RockPaperScissorsGame(GameConfig, int);
        string determineWinner() override;
        void processPlayerChoice(networking::Connection, const string&, networking::Server&) override;
        void resetPlayerChoices() override;
        string getGameName()const override;
        void sendingMessage(networking::Server&, networking::Connection, const string&) const override;
        // void addPlayer(const networking::Connection&, Player);
        // void removePlayer(const networking::Connection&);
        // int getPlayerCount();
        string toLowerCase(const string&);
        void waitInRoom(networking::Connection);
    private:
        const GameConfig& config;
        int maxRound;
        bool bothPlayerHaveChosen();
        bool isValidChoice(const string&);
        int size_of_player;
        queue<networking::Connection> waitingList;
        map<string, string> rules;
        map<networking::Connection, Player, ConnectionComparator> players;       
};