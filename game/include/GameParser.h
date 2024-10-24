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

using namespace std;

class GameConfig {
public:
    GameConfig(const string& configFileContent);

    string getGameName() const;
    pair<int, int> getPlayerRange() const;
    bool hasAudience() const;
    string readFileContent(const string& filePath);
    map<string, string> getConfiguration();
    map<string, vector<pair<pair<string, string>, pair<string, string>>>> getConstants();
    map<string, vector<pair<pair<string, string>, pair<string, string>>>> getVariables();
    map<string, vector<pair<pair<string, string>, pair<string, string>>>> getPerPlayer();
    map<string, vector<pair<pair<string, string>, pair<string, string>>>> getPerAudience();
    map<string, vector<map<string, string>>> getSetup();

    // helper functions to print result to the console
    void printKeyValuePair();
    void printMap();
private:
    string gameName;
    pair<int, int> playerRange;
    bool audience;
    vector<string> toSkip = {"[", "]", ",", "{", "}", ":", "\"", "(", ")"};
    map<string, string> configuration;
    map<string, vector<pair<pair<string, string>, pair<string, string>>>> variables;
    map<string, vector<pair<pair<string, string>, pair<string, string>>>> perPlayer;
    map<string, vector<pair<pair<string, string>, pair<string, string>>>> perAudience;
    map<string, vector<pair<pair<string, string>, pair<string, string>>>> constants;  
    map<string, vector<map<string, string>>> setup;

    //void extractStringValue(const ts::Node&, const std::string&, pair<string, string> &, pair<string, string> &, std::string, map<string, vector<pair<pair<string, string>, pair<string, string>>>>&);
    void extractStringValue(const ts::Node& node, const std::string& source, pair<string, string> &str1, pair<string, string> &str2, std::string keyID, map<string, vector<pair<pair<string, string>, pair<string, string>>>>& output);
    template <typename T>
    void parseValueMap(const ts::Node&, const string&, T&);
    void setupHelper(const ts::Node&, const string&, string&, string&, const string&);
    void parseConfig(const string&);
    void parseConfigurationSection(const ts::Node&, const string&);
    void parseConstantsSection(const ts::Node&, const string&);
    void parseVariablesSection(const ts::Node&, const string&);
    void parsePerPlayerSection(const ts::Node&, const string&);
    void parsePerAudienceSection(const ts::Node&, const string&);
};
