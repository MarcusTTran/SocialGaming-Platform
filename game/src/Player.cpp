// // author: kwa132, Mtt8

#include "Player.h"
#include "GamePerPlayer.h"
// #include "GamePerAudience.h"

Player::Player(const networking::Connection &con, std::string displayName) : connection(con), displayName(displayName) {
    playerIdentity = true;
}

void Player::setPlayerIdentity(bool identity) { playerIdentity = identity; }

bool Player::getPlayerIdentity() { return playerIdentity; }

void Player::make_choice(const std::string &c) { this->choice = c; }

networking::Connection Player::getConnection() const { return connection; }

std::string Player::getChoice() const { return choice; }

int Player::getId() const { return id; }

std::string Player::getDisplayName() const { return displayName; }

bool Player::operator==(const Player &other) const { return connection.id == other.connection.id; }

void Player::addPerVariableMap(DataValue::OrderedMapType perVariableMap, bool identity) {
    identity ? playerObjects = perVariableMap : audienceObjects = perVariableMap;
};

void Player::addPlayerVariable(std::string key, DataValue value, bool identity) {
    identity ? playerObjects.emplace(std::move(key), std::move(value))
             : audienceObjects.emplace(key, std::move(value));
};

DataValue::OrderedMapType Player::getMap(bool identity) {
    if (identity) {
        playerObjects.emplace(std::string("__id__"), static_cast<int>(id));
        playerObjects.emplace(std::string("name"), std::string(displayName));
        playerObjects.emplace(std::string("connection"), connection);
        return playerObjects;
    } else {
        audienceObjects.emplace(std::string("__id__"), static_cast<int>(id));
        audienceObjects.emplace(std::string("name"), std::string(displayName));
        audienceObjects.emplace(std::string("connection"), connection);
        return audienceObjects;
    }
};

void Player::valueSetter(const std::string &key, DataValue newValue, bool identity) {
    auto begin = (identity ? playerObjects.begin() : audienceObjects.begin());
    auto end = (identity ? playerObjects.end() : audienceObjects.end());

    auto playerIt = std::find_if(begin, end, [&key](const auto &pair) { return pair.first == key; });
    if (playerIt != end) {
        playerIt->second = std::move(newValue);
    }
}