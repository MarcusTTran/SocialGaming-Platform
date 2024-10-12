// author: kwa132, Mtt8

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <cpp-tree-sitter.h>
// #include "tree_sitter/api.h" 

using namespace std;

class GameConfig {
public:
    GameConfig(const string& configFileContent);

    string getGameName() const;
    pair<int, int> getPlayerRange() const;
    bool hasAudience() const;
    map<string, string> getConfiguration();
    map<string, vector<pair<pair<string, string>, pair<string, string>>>> getConstants();
    map<string, string> getVariables();
    map<string, string> getPerPlayer();
    map<string, string> getPerAudience();
    map<string, vector<map<string, string>>> getSetup();
private:
    string gameName;
    pair<int, int> playerRange;
    bool audience;
    vector<string> toSkip = {"[", "]", ",", "{", "}", ":", "\"", "(", ")"};
    map<string, string> configuration;
    map<string, string> variables;
    map<string, string> perPlayer;
    map<string, string> perAudience;
    map<string, vector<pair<pair<string, string>, pair<string, string>>>> constants;  
    map<string, vector<map<string, string>>> setup;

    string extractStringValue(const ts::Node&, const string&);
    void extractStringValue2(const ts::Node&, const std::string&, pair<string, string> &, pair<string, string> &, std::string);
    template <typename T>
    void parseValueMap(const ts::Node&, const string&, T&);
    void setupHelper(const ts::Node&, const string&, string&, string&, const string&);
    void parseMapHelper(const ts::Node&, const string&, map<string, string>&);
    void printTree(const ts::Node&, const string&, int);
    void parseConfig(const string&);
    void parseConfigurationSection(const ts::Node&, const string&);
    void parseConstantsSection(const ts::Node&, const string&);
    void parseVariablesSection(const ts::Node&, const string&);
    void parsePerPlayerSection(const ts::Node&, const string&);
    void parsePerAudienceSection(const ts::Node&, const string&);
};
