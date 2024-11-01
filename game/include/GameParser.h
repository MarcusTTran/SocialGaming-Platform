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
 

class ParsedGameData {
public:
    ParsedGameData(const std::string& configFileContent);

    std::string getGameName() const;
    std::pair<int, int> getPlayerRange() const;
    bool hasAudience() const;
    std::string readFileContent(const std::string& filePath);
    Configuration getConfiguration();
    const std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>>& getConstants() const;
    std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>> getVariables();
    std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>> getPerPlayer();
    std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>> getPerAudience();
    const std::vector<std::map<std::string, std::vector<std::map<std::string, std::string>>>>& getSetup() const;
    std::vector<Rule> getRules();

    // helper functions to print result to the console
    void printKeyValuePair();
private:
    std::string gameName;
    std::pair<int, int> playerRange;
    bool audience;
    Configuration configuration;
    std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>> variables;
    std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>> perPlayer;
    std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>> perAudience;
    std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>> constants;  
    // might not need this setup here since it is definied in Configuration struct
    std::vector<std::map<std::string, std::vector<std::map<std::string, std::string>>>> setup;
    std::vector<Rule> rules;

    void extractStringValue(const ts::Node& node, const std::string& source, std::pair<std::string, std::string> &str1, std::pair<std::string, std::string> &str2, std::string keyID, std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>>& output);
    template <typename T>
    void parseValueMap(const ts::Node&, const std::string&, T&);
    void setupHelper(const ts::Node& node, const std::string& source, std::string& str1, std::string& str2, const std::string& keyID
    , std::map<std::string, std::vector<std::map<std::string, std::string>>> &inputSetup);
    void parseConfig(const std::string&);
    void parseConfigurationSection(const ts::Node&, const std::string&);
    void parseConstantsSection(const ts::Node&, const std::string&);
    void parseVariablesSection(const ts::Node&, const std::string&);
    void parsePerPlayerSection(const ts::Node&, const std::string&);
    void parsePerAudienceSection(const ts::Node&, const std::string&);
    void DFS(const ts::Node& node, const std::string& source, Rule& rule);
    void handleForRule(const ts::Node& node, const std::string& source, Rule& outerRule);
    void handleMessageSection(const ts::Node& node, const std::string& source, Rule& outerRule);
    void traverseHelper(const ts::Node& node, const std::string& source, Rule& rule);
    void handleMatchRule(const ts::Node& node, const std::string& source, Rule& outerRule);
    void handleWhileSection(const ts::Node& node, const std::string& source, Rule& outerRule);
    void parseRuleSection(const ts::Node& node, const std::string& source, Rule& outerRule);
    std::string ruleTypeToString(Rule::Type type);
    Rule::Type getRuleType(const std::string& type);

    // print tree strucutre to console for debugging
    void printTree(const ts::Node& node, const std::string& source, int indent = 0);
};
