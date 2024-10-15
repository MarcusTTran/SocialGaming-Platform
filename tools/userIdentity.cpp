#include <iostream>
#include <string>
#include <chrono>
#include <random>
#include <functional>
#include <boost/dll.hpp>
#include <unordered_map>
#include <fstream>
#include <optional>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include "userIdentity.h"

// TODO: get rid of std::string for proper string objects that represent our data.

std::unordered_map<std::size_t, UserSession> userHashSessionData;

// generate our hash code to uniquely identify players along with other player data.
void generateUniqueCode(const std::string &playerName, const std::string &lobbyId)
{
    auto now = std::chrono::system_clock::now();

    userHash hashing;
    UserSession session{playerName, lobbyId, now}; // save this for server
    std::size_t hashedResult = hashing.generateHash(playerName, lobbyId, now);

    userHashSessionData[hashedResult] = session;
    saveHashToUserMachince(userHashSessionData[hashedResult], hashedResult);
}

// given a hash, look to see if a player has been given that hash.
std::optional<UserSession> findPlayerHash(std::size_t userHash)
{

    if (UserSession.find(userHash) != UserSession.end())
    {
        return userHashSessionData[hash];
    }

    // std::nullopt can be used to create any (empty) std::optional
    return std::nullopt;
}

// write the hash file to users local machine which will contain users hash.
void saveHashToUserMachince(const UserSession &userSesh, std::size_t userHash)
{
    const char *homeDir = getenv("HOME");
    if (homeDir == NULL)
    {
        homeDir = getpwuid(getuid())->pw_dir;
    }//https://stackoverflow.com/questions/35570698/accessing-linux-user-home-directory-with-fstream

    std::string filePath = homeDir + "/userStoredHash.txt";
    // store this location as it will be needed to look up users file.
    userSesh.userFilePathSaveLocation = filePath;

    std::ofstream outFile(filePath);
    if (outFile.is_open())
    {
        outFile << userHash;
        outFile.close();
    }
    else
    {
        std::cout << "Error: Unable to open file for writing at "
                  << filePath << '\n';
    }
}

// use to read hash from users file, see line 29 and 46 to see how call can work.
std::optional<std::size_t> readHashFromFile(const UserSession &userSesh)
{
    std::ifstream inFile(userSesh.userFilePathSaveLocation);
    std::size_t hash;
    
        if (inFile.is_open())
        {
            inFile >> hash;
            inFile.close();
            return hash;
        }
        else
        {
            std::cout << "Error: Unable to open file for reading at " << filePath << '\n';
            return std::nullopt;
        }
}