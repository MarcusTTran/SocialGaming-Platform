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
#include <optional>
#include "RuleTypes.h"
#include "CommonVariantTypes.h"
#include <span>


/*
    This is the class for general game's rules implementations
*/


using loopVariableType = std::variant<
    std::string,
    int,
    std::string_view,
    std::pair<std::string, int>,
    std::pair<std::string, std::string>,
    std::map<std::string, std::variant<int, std::string>>,                 
    std::shared_ptr<std::unordered_map<std::string, int>>,
    std::unordered_map<std::string, int>                
>;


class RulesRunner {
public:
    RulesRunner() {}
    void processRule(const std::vector<Rule>& rules, std::unordered_map<std::string, std::string>& loopVariables);
    std::string extractPlaceholders(const Rule& rule);
    void printMessageWithPlaceholders(const Rule& rule, const std::string& key, const std::unordered_map<std::string, std::string>& loopVariables);
    std::string ruleTypeToString(Rule::Type type);
    void implementForRule(const Rule& rule, std::unordered_map<std::string, std::string>& loopVariables);
    void printRule(const Rule& rule, int indent);

    // Setter functions
    void configurationSetter(const Configuration& config);
    void variablesSetter(const DataValue::OrderedMapType& vars);
    void constantsSetter(const DataValue::OrderedMapType& consts);
    void perPlayerSetter(const DataValue::OrderedMapType& perPlayers);
    void perAudienceSetter(const DataValue::OrderedMapType& perAudiences);
    void rulesSetter(const std::vector<Rule>& r);
    DataValue::OrderedMapType constantsGetter();
    void processRules();
private:
    DataValue::OrderedMapType variables;
    DataValue::OrderedMapType perPlayer;
    DataValue::OrderedMapType perAudience;
    DataValue::OrderedMapType constants;
    std::vector<DataValue::OrderedMapType> setup;
    std::vector<Rule> rules;
    Configuration configuration;  // instance of configuration struct
};
