// // author: kwa132, Mtt8

#include "Player.h"
// #include "GamePerPlayer.h"
// #include "GamePerAudience.h"

Player::Player(const networking::Connection &con, std::string displayName)
    : connection(con), displayName(displayName) {}

void Player::make_choice(const std::string &c) { this->choice = c; }

networking::Connection Player::getConnection() const { return connection; }

std::string Player::getChoice() const { return choice; }

variantType Player::getId() const { return id; }

std::string Player::getDisplayName() const { return displayName; }

bool Player::operator==(const Player &other) const { return connection.id == other.connection.id; }
