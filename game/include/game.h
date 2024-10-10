// author: kwa132, Mtt8

#pragma once

#include <string>
#include <map>
#include "Player.h"

using namespace std;

// class Game{
//     public:
//         virtual ~Game() = default;
//         virtual string determineWinner() = 0;
//         virtual void processPlayerChoice(networking::Connection, const string&, networking::Server&) = 0;
//         virtual void resetPlayerChoices() = 0;
//         virtual string getGameName() const = 0;
//         virtual void sendingMessage(networking::Server&, networking::Connection, const string&) const = 0;
// };

class Game{
    public:
        virtual ~Game() = default;  // Default destructor

        // Getters for important game data
        virtual GameConfiguration& getConfiguration() const = 0;
        virtual GameConstants& getConstants() const = 0;
        virtual GameVariables& getVariables() const = 0;
        virtual GamePerPlayer& getPerPlayer() const = 0;
        virtual GamePerAudience& getPerAudience() const = 0;
        virtual GameRules& getRules() const = 0;


        // virtual string determineWinner() = 0;


        // TODO: I feel like we should move this code to a Manager class. 
        virtual void sendingMessage(networking::Server&, networking::Connection, const string&) const = 0;
        virtual void processPlayerChoice(networking::Connection, const string&, networking::Server&) = 0;
        virtual void resetPlayerChoices() = 0;
};