// author: kwa132, Mtt8
#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <fmt/core.h>
#include <regex>
#include <algorithm>
#include <variant>
#include <cpp-tree-sitter.h>
#include "tree_sitter/api.h" 
#include "RuleTypes.h"

/*
    This is game parser class, which is responsible for parsing data from txt input file
    via tree-sitter and extract, store into certain data structure.
*/
 
using std::map;
using std::vector;
using std::pair;
using std::string;

class ParsedGameData {
public:
    ParsedGameData(const string& configFileContent);

    string getGameName() const;
    pair<int, int> getPlayerRange() const;
    bool hasAudience() const;
    string readFileContent(const string& filePath);
    Configuration getConfiguration();
    // map<string, vector<map<string,string> > >
    const map<string, vector<pair<pair<string, string>, pair<string, string>>>>& getConstants() const;
    map<string, vector<pair<pair<string, string>, pair<string, string>>>> getVariables();
    map<string, vector<pair<pair<string, string>, pair<string, string>>>> getPerPlayer();
    map<string, vector<pair<pair<string, string>, pair<string, string>>>> getPerAudience();
    const vector<map<string, vector<map<string, string>>>>& getSetup() const;
    vector<Rule> getRules();

    // helper functions to print result to the console
    void printKeyValuePair();
private:
    string gameName;
    pair<int, int> playerRange;
    bool audience;
    Configuration configuration;
    map<string, vector<pair<pair<string, string>, pair<string, string>>>> variables;
    map<string, vector<pair<pair<string, string>, pair<string, string>>>> perPlayer;
    map<string, vector<pair<pair<string, string>, pair<string, string>>>> perAudience;  // Note: constants, variables, perplayer, and peraudience all utilize
    map<string, vector<pair<pair<string, string>, pair<string, string>>>> constants;    //       a vector of 2 pairs inside: { outerpair{pair1, pair2}, ... }
    vector<map<string, vector<map<string, string>>>> setup;
    vector<Rule> rules;

    void extractStringValue(const ts::Node& node, const string& source, pair<string, string> &str1, pair<string, string> &str2, string keyID, map<string, vector<pair<pair<string, string>, pair<string, string>>>>& output);
    template <typename T>
    void parseValueMap(const ts::Node&, const string&, T&);
    void setupHelper(const ts::Node& node, const string& source, string& str1, string& str2, const string& keyID
    , map<string, vector<map<string, string>>> &inputSetup);
    void parseConfig(const string&);
    void parseConfigurationSection(const ts::Node&, const string&);
    void parseConstantsSection(const ts::Node&, const string&);
    void parseVariablesSection(const ts::Node&, const string&);
    void parsePerPlayerSection(const ts::Node&, const string&);
    void parsePerAudienceSection(const ts::Node&, const string&);
    void DFS(const ts::Node& node, const string& source, Rule& rule);
    void handleForRule(const ts::Node& node, const string& source, Rule& outerRule);
    void handleMessageSection(const ts::Node& node, const string& source, Rule& outerRule);
    void traverseHelper(const ts::Node& node, const string& source, Rule& rule);
    void handleMatchRule(const ts::Node& node, const string& source, Rule& outerRule);
    void handleWhileSection(const ts::Node& node, const string& source, Rule& outerRule);
    void parseRuleSection(const ts::Node& node, const string& source, Rule& outerRule);
    string ruleTypeToString(Rule::Type type);
    Rule::Type getRuleType(const string& type);

    // print tree strucutre to console for debugging
    void printTree(const ts::Node& node, const string& source, int indent = 0);
};
