// author: kwa132, Mtt8

#pragma once
#include "Client.h"
#include "Server.h"
#include <iostream>
#include <string>
#include <map>
#include <vector>
using namespace std;

class Player{
    public:
        Player();
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
};