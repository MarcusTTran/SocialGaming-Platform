// author: kwa132, Mtt8

#pragma once
#include "Client.h"
#include "Server.h"
#include <iostream>
#include <string>
#include <map>
#include <vector>
using namespace std;

template <typename T, typename M> // M is for player, T is for audience
class Player{
    public:
        Player();
<<<<<<< HEAD
        Player(networking::Connection, int ID, T audienceVar, M playerVar);
        ~Player();
        void make_choice(const string&);
        networking::Connection getConnection();
        // int roundGetter();
        string getChoice() const;
        int getId() const;

        bool inAudience();
        GamePerPlayer<M>& getPerPlayer();
        GamePerAudience<T>& getPerAudience();
    
    private:
        networking::Connection connection;
        string choice;
        int id;

        bool inAudience;
        GamePerAudience<T> audience; 
        GamePerPlayer<M> players; 
=======
        Player(networking::Connection, const string&);
        ~Player();
        void make_choice(const string&);
        networking::Connection getConnection() const;
        int roundGetter();
        void one_round_drop();
        string getChoice() const;
        int getId() const;
        string getDisplayName() const;
    private:
        const networking::Connection connection;
        int round;
        string choice;
        int id;
        string displayName;
>>>>>>> 464115116f301a5b027831865f208f0a785170c2
};