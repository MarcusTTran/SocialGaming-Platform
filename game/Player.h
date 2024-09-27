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
        Player(networking::Connection, int, int);
        ~Player();
        void make_choice(const string&);
        networking::Connection getConnection();
        int roundGetter();
        void one_round_drop();
        string getChoice() const;
        int getId() const;
    private:
        networking::Connection connection;
        int round;
        string choice;
        int id;
};