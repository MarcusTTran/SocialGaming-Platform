// author: kwa132, Mtt8
#pragma once

#include "CommonVariantTypes.h"
#include "Expression.h"
#include "Messenger.h"
#include "Rule.h"
#include "RuleTypes.h"
#include "tree_sitter/api.h"
#include <algorithm>
#include <cpp-tree-sitter.h>
#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <variant>
#include <vector>


/*
    This is game parser class, which is responsible for parsing data from txt input file
    via tree-sitter and extract, store into certain data structure.
*/

using std::map;
using std::pair;
using std::string;
using std::vector;

class ParsedGameData {
public:
    ParsedGameData(const string &configFileContent, std::shared_ptr<IServer> server,
        networking::Connection connection);

    string getGameName() const;
    pair<int, int> getPlayerRange() const;
    bool hasAudience() const;
    string readFileContent(const string &filePath);
    Configuration getConfiguration();

    // DataValue::OrderedMapType = vector<pair<string, DataValue>>
    const DataValue::OrderedMapType &getConstants() const;
    const DataValue::OrderedMapType &getVariables() const;
    const DataValue::OrderedMapType &getPerPlayer() const;
    const DataValue::OrderedMapType &getPerAudience() const;
    const std::vector<DataValue::OrderedMapType> &getSetup() const;
    const vector<std::unique_ptr<Rule>> &getRules() const;

    // For moving ownership of rules to Game object
    vector<std::unique_ptr<Rule>> moveRules();

    // helper functions to print result to the console
    // Note that anything related to print out will be removed eventually
    void printKeyValuePair();
    void printDataValue(const DataValue::OrderedMapType &value, int indent = 0);

private:
    string gameName;
    pair<int, int> playerRange;
    bool audience;
    Configuration configuration;
    std::shared_ptr<IServer> server; // For constructing messaging rules
    networking::Connection gameConnection;

    // using variant types to do a map-like data structure while preserving data order
    DataValue::OrderedMapType variables;
    DataValue::OrderedMapType perPlayer;
    DataValue::OrderedMapType perAudience;
    DataValue::OrderedMapType constants; // for RPS -> a vector of 2 pairs inside: { outerpair{pair1, pair2}, ... }
    vector<std::unique_ptr<Rule>> rules;

    DataValue handleExpression(const ts::Node &node, const std::string &source);
    void parseValueMap(const ts::Node &, const std::string &source, DataValue::OrderedMapType &output);
    DataValue::OrderedMapType handleSetup(const ts::Node &node, const std::string &source);
    void parseConfig(const string &);
    void parseConfigurationSection(const ts::Node &, const string &);
    void parseConstantsSection(const ts::Node &, const string &);
    void parseVariablesSection(const ts::Node &, const string &);
    void parsePerPlayerSection(const ts::Node &, const string &);
    void parsePerAudienceSection(const ts::Node &, const string &);
    void DFS(const ts::Node &node, const string &source, std::vector<std::string> &str);
    std::unique_ptr<Rule> handleForRule(const ts::Node &node, const string &source);
    std::unique_ptr<Rule> handleMessageSection(const ts::Node &node, const string &source);
    void traverseHelper(const ts::Node &node, const string &source, std::vector<std::unique_ptr<Rule>> &checkCondition,
                        std::vector<std::unique_ptr<Rule>> &scopedRule);
    std::unique_ptr<Rule> handleMatchRule(const ts::Node &node, const string &source);
    void handleWhileSection(const ts::Node &node, const string &source, Rule &outerRule);
    std::unique_ptr<Rule> handelInputChoice(const ts::Node &node, const std::string &source);
    // void handleNameResolverRule(const ts::Node &node, const string &source, Rule &outerRule);
    std::unique_ptr<Rule> handleBuiltin(const ts::Node &node, const std::string &source, std::unique_ptr<Rule> rule
        , std::vector<DataValue> list);
    std::unique_ptr<Rule> handleDiscard(const ts::Node &node, const std::string &source);
    std::unique_ptr<Rule> handleScore(const ts::Node &node, const std::string &source);
    std::unique_ptr<Rule> parseRuleSection(const ts::Node &node, const string &source);
    std::unique_ptr<Rule> handleExtend(const ts::Node &node, const std::string &source);
    string ruleTypeToString(RuleT::Type type);
    RuleT::Type getRuleType(const string &type);

    std::string extractAndReplacePlaceholders(const std::string &contentStr, std::vector<std::string> &variables);
    void splitString(const std::string &str, char delimiter, std::vector<std::string> &parts);
    // print tree strucutre to console for debugging
    void printTree(const ts::Node &node, const string &source, int indent = 0);
    void printSingleDataValue(const DataValue &value, int indent);
};