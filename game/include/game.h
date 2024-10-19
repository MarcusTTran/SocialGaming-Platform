// author: kwa132, Mtt8
#ifndef GAME_H
#define GAME_H

#pragma once

#include <string>
#include <map>
#include <vector>
#include "GameConfiguration.h"
#include "GameConstants.h"
#include "GameVariables.h"
#include "GameRules.h"
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

class Game {
    GameConfiguration configuration;
    GameConstants constants;
    GameVariables variables;
    GameRules rules;
    vector<Player> players;

    public:
<<<<<<< HEAD
        // Constructor (cannot be instantiated without all provided fields ie. no default constructor allowed)
        Game(GameConfiguration& configuration, GameConstants& constants, GameVariables& variables, GamePerAudience& perAudience, GameRules & rules, vector<Player> players)
            : configuration(configuration), constants(constants), variables(variables), perAudience(perAudience), rules(rules), players(players) {
                //TODO: Perhaps deep copy Players vector?
            }

        ~Game() = default;  // Default destructor

        // Getters for important game data
        GameConfiguration& getConfiguration();
        GameConstants& getConstants();
        GameVariables& getVariables();
        GameRules& getRules();
        vector<Player>& getPlayers();


        // TODO: I feel like we should move this code to a Manager class. 
        // void sendingMessage(networking::Server&, networking::Connection, const string&);
        // void processPlayerChoice(networking::Connection, const string&, networking::Server&);
        // void resetPlayerChoices();
};

#endif
=======
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

>>>>>>> 464115116f301a5b027831865f208f0a785170c2
