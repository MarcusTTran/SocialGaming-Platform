// author: kwa132, Mtt8

#include "Player.h"
#include "GamePerPlayer.h"
#include "GamePerAudience.h"


template <typename T, typename M>
Player<T, M>::Player() : connection(), choice(""), id(0) {
    audience = GamePerAudience<T>();
    player = GamePerPlayer<T>();
}

template <typename T, typename M>
Player<T, M>::Player(networking::Connection conn, int round, int ID, T audienceVar, T playerVar) : connection(conn), round(round), id(ID) {

}

template <typename T, typename M>
Player<T, M>::~Player(){}

template <typename T, typename M>
bool Player<T, M>::inAudience() {
    return inAudience;
}

template <typename T, typename M>
void Player<T, M>::make_choice(const string& c) {this->choice = c;}

template <typename T, typename M>
networking::Connection Player<T, M>::getConnection(){
    return connection;
}

template <typename T, typename M>
int Player<T, M>::roundGetter(){
    return round;
}

template <typename T, typename M>
void Player<T, M>::one_round_drop(){
    round--;
}

template <typename T, typename M>
string Player<T, M>::getChoice() const{
    return choice;
}

template <typename T, typename M>
int Player<T, M>::getId() const{
    return id;
}

template <typename T, typename M>
GamePerPlayer<M>& Player<T, M>::getPerPlayer() {
    return players;
}

template <typename T, typename M>
GamePerAudience<T>& Player<T, M>::getPerAudience() {
    return audience;
}


