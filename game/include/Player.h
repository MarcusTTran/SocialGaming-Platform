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
        Player(networking::Connection, int round, int ID, T audienceVar, T playerVar);
        ~Player();
        void make_choice(const string&);
        networking::Connection getConnection();
        int roundGetter();
        void one_round_drop();
        string getChoice() const;
        int getId() const;

        bool inAudience();
        GamePerPlayer<M>& getPerPlayer();
        GamePerAudience<T>& getPerAudience();
    
    private:
        networking::Connection connection;
        int round;
        string choice;
        int id;

        bool inAudience;
        GamePerAudience<T> audience; 
        GamePerPlayer<M> players; 
};