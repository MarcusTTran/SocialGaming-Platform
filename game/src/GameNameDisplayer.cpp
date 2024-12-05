#include "GameNameDisplayer.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

/* This function will aquire all the game names in the config folder(looks for first "name:" and acquires game name from
that). Specifically game names that reside in configuration{}, where configuration{} resides in the game config file
doesn't matter. This functions logic assumes its only called once during lifetime of program. Additionally, when there
is an issue trying to parse a game config file, it will add the issue to the output string. However,
... it should be noted that those config files that have issues will not be added to the configmap, so only valid game
configs will in map. */
/* Sample Output assuming 4 config files(2 have same game)--
Game names:
1: Rock, Paper, Scissors
2: Rock, Paper, Scissors
3: config in different order game
4: minimal */

/* Sample output that has a problem config along with print out of config map:
CAUGHT EXCEPTION: The config file is empty:  ../../config/emptyConfig.game
Game names:
1: Rock, Paper, Scissors
2: Rock, Paper, Scissors
3: config in different order game
4: minimal
Key: 1 value: ../../config/config.game
Key: 2 value: ../../config/rockPaperScissors.game
Key: 3 value: ../../config/testDisplay.game
Key: 4 value: ../../config/minimal.game */
std::string gameNameDisplayer() {
    std::string configFolder =
        "../config"; // path from build/bin directory.NOTE: must run game_server from bin directory for this to work.
    std::vector<std::string> gameNames;
    std::ostringstream output;

    try {
        int count = 1; // used to simulate order that will be same as when we print out to user at end of this function.
        // Iterate through all files in the folder
        for (const auto &entry : std::filesystem::directory_iterator(configFolder)) {
            if (entry.is_regular_file()) {
                try {
                    std::string gameName = gameNameExtraction(entry.path().string());
                    if (!gameName.empty()) {
                        gameNames.push_back(gameName);
                        getConfigMap().emplace(count, entry.path().string()); // populate the map
                        count++;
                    }
                } catch (const std::runtime_error &e) {
                    output << "CAUGHT EXCEPTION: " << e.what() << '\n';
                }
            }
        }
    } catch (const std::filesystem::filesystem_error &e) {
        output << "Filesystem error trying to read config files: " << e.what() << "\n";
        return output.str();
    }

    // Display the game names
    output << "Game names:\n";
    int counter = 1;
    for (const auto &name : gameNames) {
        output << counter << ": " << name << "\n";
        counter++;
    }
    return output.str();
}

/* The purpose of this function is to get the mapping of game to order that is printed to player from the
gameNameDisplayer. So then when asking for player input, they input integer of game they want i.e "1" and then you can
just use that to match it with proper key in map. You can use this function as a entry to this map for when you need to
match and then return the associated game config path so it can be used for our game parser. Static variable is used
here so this vector persists for life of program without having global variable.*/
std::map<int, std::string> &getConfigMap() {
    static std::map<int, std::string> configMap; // initialized only once on first call.
    return configMap;
}

// Extracts game name from each game config file, is utilized by gameNameDisplayer.
std::string gameNameExtraction(std::string configFolder) {

    if (std::filesystem::is_empty(configFolder)) {
        throw std::runtime_error("The config file is empty:  " + configFolder);
    }

    std::ifstream file(configFolder);
    if (!file.is_open()) {
        throw std::runtime_error("Error opening file:  " + configFolder);
    }

    std::string line, content;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    file.close();

    // Find the configuration block
    /* Conceptually what happens here is we find 'configuration {', then we look for the variable after 'name:' which is
    player range (we just used 'player' as stopping point). then we create substr from congifuration {  up to player and
    work with that to extract the game name: . this ensures we extract the correct name as there are multiple name:
    variables throughout config file. */
    size_t configStart = content.find("configuration {");
    size_t configEnd = content.find("player", configStart); // player range is always after name so instance of 'player'
                                                            // will be our stopping point for substr.
    if (configStart == std::string::npos || configEnd == std::string::npos) {
        throw std::runtime_error("Configuration block issue, couldn't acquire 'name:' from config file: " +
                                 configFolder);
    }
    // Extract configuration block
    std::string configBlock = content.substr(configStart, configEnd - configStart + 1);

    // find first name: in configuration{}, then extract game name.
    size_t gameNamePos = configBlock.find("name:");
    if (gameNamePos != std::string::npos) {
        size_t valueStart = configBlock.find("\"", gameNamePos) + 1; // first letter of word will be acquired.
        size_t valueEnd =
            configBlock.find("\"", valueStart); // last double quote encasing game name will be acquired here.
        if (valueStart != std::string::npos && valueEnd != std::string::npos) {
            return configBlock.substr(valueStart,
                                      valueEnd - valueStart); // Game name acquired here and is value we return.
        }
    }
    throw std::runtime_error("Reached end of gameNameExtraction which means we failed to acquire a game name, config "
                             "file we tried to extract from:  " +
                             configFolder);
}