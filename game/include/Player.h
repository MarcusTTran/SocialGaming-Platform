// author: kwa132, Mtt8
#pragma once
#include "Client.h"
#include "GamePerAudience.h"
#include "GamePerPlayer.h"
#include "Server.h"
#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <variant>
#include <vector>


class Player {
public:
  Player(const networking::Connection &con, std::string displayName);
  void make_choice(const std::string &);
  networking::Connection getConnection() const;
  std::string getChoice() const;
  int getId() const;
  std::string getDisplayName() const;
  void addPerVariableMap(DataValue::OrderedMapType perVariableMap, bool identity);
  DataValue::OrderedMapType getMap(bool identity);
  void valueSetter(const std::string& key, DataValue newValue, bool identity);
  void addPlayerVariable(std::string key, DataValue value, bool identity);
  void setPlayerIdentity(bool identity);
  bool getPlayerIdentity();

private:
  std::string displayName;
  networking::Connection connection;
  std::string choice;
  int id;
  DataValue::OrderedMapType playerObjects;
  DataValue::OrderedMapType audienceObjects;
  bool playerIdentity;
};