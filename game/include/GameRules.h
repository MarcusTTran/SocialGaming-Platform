#ifndef GAMERULES_H
#define GAMERULES_H

#include "RuleTypes.h"
#include <string>
#include <vector>

// - Contains the structure of the game using a social gaming file language as pseudocode
// - High-level control statements include: for, while, parallel for, in parallel, match
// - Other statements may start with an operation like "discard" or "message all"
// - Often, the rules will refer to other variabels or constants define in other parts of the game file "ie.
// configuration.rounds.upFrom(1)"

// IMPORTANT: KEEP VECTOR OF RULES IN-ORDER! IT REPRESENTS A LINEAR SEQUENCE OF OPERATIONS TO RUN THE GAME

// enum GameRuleType {
//     // FOR,
//     // WHILE,
//     // PARALLEL_FOR,
//     // IN_PARALLEL,
//     MATCH,
//     LIST_OP,
//     INPUT,
//     OUTPUT,
//     ASSIGNMENT,
//     STATEMENT
// };

// enum builtInFunctionName {
//     UPFROM,
//     SIZE,
//     CONTAINS,
//     COLLECT
// };

/* Example of 1 rule maybe?

for round in configuration.rounds.upfrom(1) {

    discard winners.size() from winners;

    message all "Round {round}. Choose your weapon!";

    parallel for player in players {
      input choice to player {
        prompt: "{player.name}, choose your weapon!"
        choices: weapons.name
        target: player.name
        timeout: 10
      }
    }
}
*/

// Make this an interface
// class Rule {
//     // public:
//     //     virtual ~Rule() = default;
//     //     virtual GameRuleType getType() const = 0;
//     //     virtual bool isNested() const = 0;
// };

class NestedRule : public Rule {
    // GameRuleType type;
    // std::string statementBody;
    // std::optional<builtInFunctionName> builtinFunction;
    // std::optional<std::string> containerName;
    // std::optional<std::string> iteratorName;
    // std::vector<Rule> nestedRules;

    // public:
    //     GameRuleType getType() const override {
    //         return type;
    //     }
};

class StatementRule : public Rule {
    // GameRuleType type;
    // std::string statementBody;
    // std::optional<builtInFunctionName> builtinFunction;
    // std::optional<std::string> containerName;
    // std::optional<std::string> iteratorName;

    // public:
    //     GameRuleType getType() const override {
    //         return type;
    //     }
};

class GameRules {
    // std::vector<Rule> rules;

    // public:
    //     // GameRules();
    //     ~GameRules();
    //     std::vector<Rule> getRules();
};

#endif // GAMERULES_H
