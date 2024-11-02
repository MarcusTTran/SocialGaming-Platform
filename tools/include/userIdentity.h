#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <boost/dll.hpp>
#include <optional>
struct userHash
{
    std::size_t generateHash(
        const std::string &userId,
        const std::string &lobbyId,
        const std::chrono::system_clock::time_point joinedLobbyTimeStamp)
    {
        std::size_t userH = std::hash<std::string>{}(userId);
        std::size_t lobbyH = std::hash<std::string>{}(lobbyId);

        // have to cast to int64_T to give has a type it can work with.
        auto ms_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(joinedLobbyTimeStamp.time_since_epoch()).count();
        std::size_t timeH = std::hash<std::int64_t>{}(ms_since_epoch);

        std::size_t seed = 0;
        boost::hash_combine(seed, userH);
        boost::hash_combine(seed, lobbyH);
        boost::hash_combine(seed, timeH);
        return seed;
    }
};

// used by server to associate player information with a hash later.
struct UserSession
{
    std::string userName;
    std::string lobbyId;
    std::chrono::system_clock::time_point joinTime;
    std::string userFilePathSaveLocation;
};


// generate our hash code to uniquely identify players along with other player data.
void generateUniqueCode(const std::string &playerName, const std::string &lobbyId);

// given a hash, look to see if a player has been given that hash.
std::optional<UserSession> findPlayerHash(std::size_t userHash);

void saveHashToUserMachince(const UserSession& userSesh,std::size_t userHash);
std::optional<std::size_t> readHashFromFile(const UserSession &userSesh);
