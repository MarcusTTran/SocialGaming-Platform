// // author: kwa132, Mtt8

#include "Player.h"
// #include "GamePerPlayer.h"
// #include "GamePerAudience.h"

Player::Player(networking::Connection con, variantType ID) :
connection(con), id(ID){}

void Player::make_choice(const std::string & c){
    this->choice = c;
}

networking::Connection Player::getConnection() const{
    return connection;
}

std::string Player::getChoice() const{
    return choice;
}

variantType Player::getId() const{
    return id;
}

std::string Player::getDisplayName(){
    return "";
}

