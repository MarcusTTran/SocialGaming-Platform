// author: kwa132, Mtt8

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <cpp-tree-sitter.h>
#include "tree_sitter/api.h" 


class GameConfig {
public:
    GameConfig(const std::string& configFileContent);

    std::string getGameName() const;
    std::pair<int, int> getPlayerRange() const;
    bool hasAudience() const;
    std::string readFileContent(const std::string& filePath);
    std::map<std::string, std::string> getConfiguration();
    std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>> getConstants();
    std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>> getVariables();
    std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>> getPerPlayer();
    std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>> getPerAudience();
    std::map<std::string, std::vector<std::map<std::string, std::string>>> getSetup();

    // helper functions to print result to the console
    void printKeyValuePair();
    void printMap();
private:
    std::string gameName;
    std::pair<int, int> playerRange;
    bool audience;
    std::vector<std::string> toSkip = {"[", "]", ",", "{", "}", ":", "\"", "(", ")"};
    std::map<std::string, std::string> configuration;
    std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>> variables;
    std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>> perPlayer;
    std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>> perAudience;
    std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>> constants;  
    std::map<std::string, std::vector<std::map<std::string, std::string>>> setup;

    //void extractStringValue(const ts::Node&, const std::string&, pair<string, string> &, pair<string, string> &, std::string, map<string, vector<pair<pair<string, string>, pair<string, string>>>>&);
    void extractStringValue(const ts::Node& node, const std::string& source, std::pair<std::string, std::string> &str1, std::pair<std::string, std::string> &str2, std::string keyID, std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>>& output);
    template <typename T>
    void parseValueMap(const ts::Node&, const std::string&, T&);
    void setupHelper(const ts::Node&, const std::string&, std::string&, std::string&, const std::string&);
    void parseConfig(const std::string&);
    void parseConfigurationSection(const ts::Node&, const std::string&);
    void parseConstantsSection(const ts::Node&, const std::string&);
    void parseVariablesSection(const ts::Node&, const std::string&);
    void parsePerPlayerSection(const ts::Node&, const std::string&);
    void parsePerAudienceSection(const ts::Node&, const std::string&);
};
