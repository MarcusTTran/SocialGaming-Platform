// author: kwa132, Mtt8

#pragma once
#include "Client.h"
#include "Server.h"
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
  // int roundGetter();
  string getChoice() const;
  int getId() const;

  bool inAudience();
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