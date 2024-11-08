#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>
#include "CommonVariantTypes.h"

/*
    This is type class, which is used for parser and rules
*/

namespace GameConstantsType {
    inline const std::vector<std::string> toSkip = {"[", "]", ",", "{", "}", ":", "\"", "(", ")", ".", ";"};
    inline const std::vector<std::string> builtin = {"upfrom", "size", "contains", "collect"};
}

struct Configuration {
    std::string name = "";
    bool audience = false;  
    std::pair<int, int> range = {0, 0};  
    std::vector<DataValue::OrderedMapType> setup;

    Configuration() : name(""), audience(false), range({0, 0}), setup({}) {}
};

struct Rule {
    enum class Type {
        Default, For, Loop, ParallelFor, InParallel, Match, Extend, Reverse, Shuffle, Sort, Deal,
        Discard, Timer, InputChoice, InputText, InputVote, InputRange, Message, Scores,
        Assignment, Body
    };
    Type type = Type::Default;
    std::vector<std::variant<int, std::string, bool, std::vector<std::string>, std::map<std::string, std::variant<int, std::string, bool, std::vector<std::string>>>>> parameters;
    std::vector<Rule> subRules;
};

