#pragma once
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <string_view>
#include <fmt/core.h>
#include <regex>
#include <vector>
#include <algorithm>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <variant>
#include <cassert>
#include "GameTypes.h"
#include <span>


/*
    This is the class for general game's rules implementations
*/


using generalType = std::map<std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>>>;
using loopVariableType = std::variant<
    std::string,
    int,
    std::string_view,
    std::pair<std::string, int>,
    std::pair<std::string, std::string>,
    std::map<std::string, std::variant<int, std::string>>,                 
    std::shared_ptr<std::unordered_map<std::string, int>>                  
>;


class RulesRunner {
public:
    RulesRunner() {}
    void processRule(const std::vector<Rule>& rules);
    std::unordered_map<std::string, std::string> extractPlaceholders(const Rule& rule);
    void printMessageWithPlaceholders(const Rule& rule, const std::unordered_map<std::string, std::string>& values);
    std::string ruleTypeToString(Rule::Type type);
    void implementForRule(const Rule& rule, std::vector<loopVariableType> loopVariables);
    void printRule(const Rule& rule, int indent);

    // Setter functions
    void configurationSetter(const Configuration& config);
    void variablesSetter(const generalType& vars);
    void constantsSetter(const generalType& consts);
    void perPlayerSetter(const generalType& perPlayers);
    void perAudienceSetter(const generalType& perAudiences);
    void rulesSetter(const std::vector<Rule>& r);
    generalType constantsGetter();
    void processRules();
private:
    generalType variables;
    generalType perPlayer;
    generalType perAudience;
    generalType constants;
    std::vector<std::map<std::string, std::vector<std::map<std::string, std::string>>>> setup;
    std::vector<Rule> rules;
    Configuration configuration;  // instance of configuration struct
};
