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
#include "CommonVariantTypes.h"

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
    // vector<pair<string, DataValue>>
    const DataValue::OrderedMapType& getConstants() const;
    const DataValue::OrderedMapType& getVariables() const;
    const DataValue::OrderedMapType& getPerPlayer() const;
    const DataValue::OrderedMapType& getPerAudience() const;
    const vector<map<string, vector<map<string, string>>>>& getSetup() const;
    vector<Rule> getRules();

    // helper functions to print result to the console
    // Note that anything related to print out will be removed eventually
    void printKeyValuePair();
    void printDataValue(const DataValue::OrderedMapType& value, int indent = 0);
private:
    string gameName;
    pair<int, int> playerRange;
    bool audience;
    Configuration configuration;

    // using variant types to do a map-like data structure while preserving data order
    DataValue::OrderedMapType variables;
    DataValue::OrderedMapType perPlayer;
    DataValue::OrderedMapType perAudience;
    DataValue::OrderedMapType constants;    //       a vector of 2 pairs inside: { outerpair{pair1, pair2}, ... }
    vector<map<string, vector<map<string, string>>>> setup;
    vector<Rule> rules;

    DataValue handleExpression(const ts::Node& node, const std::string& source);
    void parseValueMap(const ts::Node& node, const std::string& source, DataValue::OrderedMapType& output);
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
    void printSingleDataValue(const DataValue& value, int indent);
};
