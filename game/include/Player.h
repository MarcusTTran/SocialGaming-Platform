// author: kwa132, Mtt8

#pragma once
#include "Client.h"
#include "Server.h"
#include <iostream>
#include <string>
#include <map>
#include <vector>
using namespace std;

template <typename T>
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

        bool isAudience();
        GamePerPlayer<T>& getPerPlayer();
        GamePerAudience<T>& getPerAudience();
    
    private:
        networking::Connection connection;
        int round;
        string choice;
        int id;

        bool inAudience;
        // vector of perplayer things
        GamePerAudience<T> audience; //maybe unique_ptr
        // vector of peraudience things
        GamePerPlayer<T> player; //maybe unique_ptr
};