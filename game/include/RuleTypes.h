#pragma once

#include "CommonVariantTypes.h"
#include <map>
#include <string>
#include <variant>
#include <vector>

/*
    This is type class, which is used for parser and rules
*/

namespace GameConstantsType {
inline const std::vector<std::string> toSkip = {"[", "]", ",", ":", "\"", "(", ")", ".", ";"};
inline const std::vector<std::string> builtin = {"upfrom", "size", "contains", "collect"};
} // namespace GameConstantsType

struct Configuration {
    std::string name = "";
    bool audience = false;
    std::pair<int, int> range = {0, 0};
    std::vector<DataValue::OrderedMapType> setup;

    Configuration() : name(""), audience(false), range({0, 0}), setup({}) {}
};

struct RuleT {
    enum class Type {
        Default,
        For,
        Loop,
        ParallelFor,
        InParallel,
        Match,
        Extend,
        Reverse,
        Shuffle,
        Sort,
        Deal,
        Discard,
        Timer,
        InputChoice,
        InputText,
        InputVote,
        InputRange,
        Message,
        Scores,
        Assignment,
        Body
    };
    Type type = Type::Default;
    std::vector<DataValue> parameters;
    std::vector<RuleT> subRules;
};
