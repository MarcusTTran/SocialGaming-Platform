// // author: kwa132, Mtt8

#include "Player.h"
// #include "GamePerPlayer.h"
// #include "GamePerAudience.h"

Player::Player(networking::Connection con, variantType ID) :
connection(con), id(ID), inAudience(false){}

void Player::make_choice(const string & c){
    this->choice = c;
}

networking::Connection Player::getConnection() const{
    return connection;
}

string Player::getChoice() const{
    return choice;
}

variantType Player::getId() const{
    return id;
}

bool Player::getAudience(){
    return inAudience;
}

string Player::getDisplayName(){
    return "";
}

