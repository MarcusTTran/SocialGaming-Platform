// author: kwa132, Mtt8

#include "Player.h"

template <typename T>
Player::Player() : connection(), round(0), choice(""), id(0) {
    audience = GamePerAudience<T>();
    player = GamePerPlayer<T>();
}

template <typename T>
Player::Player(networking::Connection conn, int round, int ID, T audienceVar, T playerVar) : connection(conn), round(round), id(ID) {

}

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