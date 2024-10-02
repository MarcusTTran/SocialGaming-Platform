// author: kwa132, Mtt8

#pragma once

#include <string>
#include <map>
#include "Player.h"

using namespace std;

class Game{
    public:
        virtual ~Game() = default;
        virtual string determineWinner() = 0;
        virtual void processPlayerChoice(networking::Connection, const string&, networking::Server&) = 0;
        virtual void resetPlayerChoices() = 0;
        virtual string getGameName() const = 0;
        virtual void sendingMessage(networking::Server&, networking::Connection, const string&) const = 0;
};