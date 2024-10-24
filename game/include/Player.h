// author: kwa132, Mtt8
#pragma once
#include "Client.h"
#include "Server.h"
#include "GamePerPlayer.h"
#include "GamePerAudience.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

template <typename T, typename M> // M is for player, T is for audience
class Player {
public:
  Player();
  Player(networking::Connection, int ID, T audienceVar, M playerVar);
  ~Player();
  void make_choice(const string &);
  networking::Connection getConnection();
  string getChoice() const;
  int getId() const;
  bool getAudience();
  GamePerPlayer<M> &getPerPlayer();
  GamePerAudience<T> &getPerAudience();

private:
  networking::Connection connection;
  string choice;
  int id;
  bool inAudience;
  GamePerAudience<T> audience;
  GamePerPlayer<M> players;
};

// Template definitions moved to the header file

template <typename T, typename M>
Player<T, M>::Player() : connection(), choice(""), id(0) {
    audience = GamePerAudience<T>();
    players = GamePerPlayer<M>();
}

template <typename T, typename M>
Player<T, M>::Player(networking::Connection conn, int ID, T audienceVar, M playerVar)
    : connection(conn), id(ID), inAudience(false) {
    this->audience = audienceVar;
    this->players = playerVar;
}

template <typename T, typename M>
Player<T, M>::~Player() {}

template <typename T, typename M>
bool Player<T, M>::getAudience() {
    return inAudience;
}

template <typename T, typename M>
void Player<T, M>::make_choice(const string &c) {
    this->choice = c;
}

template <typename T, typename M>
networking::Connection Player<T, M>::getConnection() {
    return connection;
}

template <typename T, typename M>
string Player<T, M>::getChoice() const {
    return choice;
}

template <typename T, typename M>
int Player<T, M>::getId() const {
    return id;
}

template <typename T, typename M>
GamePerPlayer<M> &Player<T, M>::getPerPlayer() {
    return players;
}

template <typename T, typename M>
GamePerAudience<T> &Player<T, M>::getPerAudience() {
    return audience;
}
