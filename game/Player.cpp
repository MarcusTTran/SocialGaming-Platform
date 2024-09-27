#include "Player.h"

Player::Player() : connection(), round(0), choice(""), id(0){}

Player::Player(networking::Connection conn, int r, int pId) : connection(conn), round(r), id(pId){}

Player::~Player(){}

void Player::make_choice(const string& c) {this->choice = c;}

networking::Connection Player::getConnection(){
    return connection;
}

int Player::roundGetter(){
    return round;
}

void Player::one_round_drop(){
    round--;
}

string Player::getChoice() const{
    return choice;
}

int Player::getId() const{
    return id;
}