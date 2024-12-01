#pragma once

#include "CommonVariantTypes.h"
#include "Messenger.h"
#include "NameResolver.h"
#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

// Example variables state
// vector([
//   { [configuration, {[name, "RPS"], [player_range, (1, 20)]}], [winners, vector(["Jake", "Mark"])] }, // Parse-time
//   { [rounds, 4] },                                                                                    // Setup-time
//   { [players, vector([...])] },                                                                       // Upon "Start
//   Game" { [round, 0] },                                                                                     //
//   Execution-time - entering for { [player, {[__id__, 23987562], [name, "Jake"], [wins, 0], [weapon, "Rock"]}] } //
//   Execution-time - entering parallel-for
// ])

// fullscope[0] = top level stuff
// fullscope[1] = for rule for RPS

using Scope = Map;

class Rule {
public:
    virtual ~Rule() = default;

    /*
     * Attempt to execute as much of the rule as possible.
     */
    DataValue runBurst(NameResolver &name_resolver) {
        std::cout << "Running rule" << std::endl;
        if (first_time) {
            _handle_dependencies(name_resolver);
        }
        first_time = false;
        name_resolver.addInnerScope();
        DataValue returnValue = _runBurst(name_resolver);
        name_resolver.removeInnerScope();
        std::cout << returnValue.getType() << std::endl;
        if (returnValue.isCompleted()) {
            first_time = true;
        }
        return returnValue;
    }

private:
    /*
     * Evaluate execution-time parameters
     */
    virtual void _handle_dependencies(NameResolver &name_resolver) = 0;
    bool first_time = true;

    virtual DataValue _runBurst(NameResolver &name_resolver) = 0;
};

class NumberRule : public Rule {
public:
    NumberRule(int number) : number(number) {}

private:
    void _handle_dependencies(NameResolver &name_resolver) override {}
    DataValue _runBurst(NameResolver &name_resolver) override { return DataValue(number); }
    const int number;
};

class BooleanRule : public Rule {
public:
    BooleanRule(bool boolean) : boolean(boolean) {}

private:
    void _handle_dependencies(NameResolver &name_resolver) override {}
    DataValue _runBurst(NameResolver &name_resolver) override { return DataValue(boolean); }
    const bool boolean;
};

class StringRule : public Rule {
public:
    StringRule(std::string_view string_literal) : string(string_literal) {}

private:
    void _handle_dependencies(NameResolver &name_resolver) override {
        // TODO: handle strings with {} braces
    }

    DataValue _runBurst(NameResolver &name_resolver) override { return DataValue(string); }

    std::string string;
};

class AllPlayersRule : public Rule {
public:
private:
    void _handle_dependencies(NameResolver &name_resolver) override {}

    DataValue _runBurst(NameResolver &name_resolver) override {
        auto playersMap = name_resolver.getValue("players");

        if (playersMap.has_value()) {
            return playersMap.value();
        } else {
            throw std::runtime_error("Players map was not found in global map");
        }
    }
};

class MessageRule : public Rule {
public:
    MessageRule(std::shared_ptr<IServer> server, std::unique_ptr<Rule> recipient_list_maker,
                std::unique_ptr<Rule> string_maker)
        : messager(server), recipient_list_maker(std::move(recipient_list_maker)),
          string_maker(std::move(string_maker)) {}

private:
    void _handle_dependencies(NameResolver &name_resolver) override {
        recipients = recipient_list_maker->runBurst(name_resolver).asOrderedMap();

        message = string_maker->runBurst(name_resolver).asString();
        std::cout << "Message: from message rule: " << message << std::endl;
    }

    DataValue _runBurst(NameResolver &name_resolver) override {
        for (const auto &[key, value] : recipients) {
            std::cout << "Sending message to: " << key << std::endl;
            messager->sendMessageToPlayerMap(message, value.asOrderedMap());
        }

        return DataValue(DataValue::RuleStatus::DONE);
    }

    std::shared_ptr<IServer> messager;
    std::unique_ptr<Rule> recipient_list_maker;
    std::unique_ptr<Rule> string_maker;
    DataValue::OrderedMapType recipients;
    std::string message;
};

// for round in configuration.rounds.upfrom(1)
// for round in configuration.rounds
class ForRule : public Rule {
public:
    ForRule(std::string fresh_variable_name, std::unique_ptr<Rule> list_maker,
            std::vector<std::unique_ptr<Rule>> contents)
        : fresh_variable_name{std::move(fresh_variable_name)}, list_maker{std::move(list_maker)}, // Move the unique_ptr
          statement_list{std::move(contents)} {}

private:
    void _handle_dependencies(NameResolver &name_resolver) override {
        auto list_of_values_generic = list_maker->runBurst(name_resolver);
        list_of_values = list_of_values_generic.asList();

        // Initialize the iterators
        value_for_this_loop = list_of_values.begin();
        current_statement = statement_list.begin();
    }

    DataValue _runBurst(NameResolver &name_resolver) override {
        // Check for early return
        if (list_of_values.empty() || statement_list.empty()) {
            return DataValue({DataValue::RuleStatus::DONE});
        }

        // Set up fresh variable
        if (list_of_values.size() > 0) {
            name_resolver.addNewValue(fresh_variable_name, *value_for_this_loop);
        }

        while (true) {
            assert(value_for_this_loop != list_of_values.end() && "Iterator for list_of_values is invalid");
            assert(current_statement != statement_list.end() && "Iterator for statement_list is invalid");

            // Run
            auto rule_state = (*current_statement)->runBurst(name_resolver); // Dereference unique_ptr
            if (rule_state.asRuleStatus() == DataValue::RuleStatus::NOTDONE) {
                return DataValue({DataValue::RuleStatus::NOTDONE});
            }

            // Move to the next statement
            current_statement++;
            if (current_statement != statement_list.end()) {
                continue;
            }

            // Move to the next full iteration
            current_statement = statement_list.begin();
            value_for_this_loop++;
            if (value_for_this_loop != list_of_values.end()) {
                name_resolver.setValue(fresh_variable_name, *value_for_this_loop);
                continue;
            }

            // Every iteration complete
            return DataValue({DataValue::RuleStatus::DONE});
        }
    }

    std::string fresh_variable_name;

    std::unique_ptr<Rule> list_maker; // Changed to unique_ptr
    std::vector<DataValue> list_of_values;
    std::vector<DataValue>::iterator value_for_this_loop;

    std::vector<std::unique_ptr<Rule>> statement_list;
    std::vector<std::unique_ptr<Rule>>::iterator current_statement;
};

// - Takes in an integer that represents how many elements we remove from the list
// - Start removal from the last element in the list
// PRECONDITION: list_maker MUST return a list, other data structures like maps are not allowed.
//               integer expression must be <= list size and > 0.
// POSTCONDITION: If integer expression <= 0 or > list size, or list is of incorrect type or empty, return -1.
//                Otherwise, return 1.
class DiscardRule : public Rule {
public:
    DiscardRule(std::unique_ptr<Rule> integer_expr_maker, std::unique_ptr<Rule> list_maker) 
        : integer_expr_maker{std::move(integer_expr_maker)}, list_maker{std::move(list_maker)} {}
    
private:
    void _handle_dependencies(NameResolver &name_resolver) override {
        integerExpression = integer_expr_maker->runBurst(name_resolver).asNumber();
        listToDiscard = list_maker->runBurst(name_resolver).asList();
    }

    DataValue _runBurst(NameResolver &name_resolver) override {
        // Check certain invariants for this functions
        bool listIsIncorrectType = listToDiscard.getType() != "LIST";
        bool numGreaterThanSize = integerExpression > listToDiscard.asList().size(); 
       
        // Ensure that list is correct type and not empty
        if (listIsIncorrectType || listToDiscard.asList().empty() || numGreaterThanSize) {
            return DataValue(DataValue::RuleStatus::ERROR);
        } 
    
        // auto& list = listToDiscard.asList();
        auto& list = const_cast<std::vector<DataValue>&>(listToDiscard.asList());
        for (size_t i = 0; i < integerExpression; ++i) {
            list.pop_back();
        }
        
        return DataValue(DataValue::RuleStatus::DONE);
    }

    std::unique_ptr<Rule> integer_expr_maker; // Some sort of (integer) expression Rule
    std::unique_ptr<Rule> list_maker;   // NameResolverRule
    int integerExpression; 
    DataValue listToDiscard;
};

class UpfromRule : public Rule {
public:
    UpfromRule(Rule &number_maker, int starting_value) : number_maker(number_maker), starting_value(starting_value) {}

private:
    void _handle_dependencies(NameResolver &name_resolver) override {
        auto ending_value_generic = number_maker.runBurst(name_resolver);
        ending_value = ending_value_generic.asNumber();
    }

    DataValue _runBurst(NameResolver &name_resolver) override {
        // return empty list, not a decreasing list
        if (ending_value < starting_value) {
            return DataValue(std::vector<DataValue>());
        }
        // construct vector
        std::vector<DataValue> list_of_ints;
        int number_of_values = ending_value - starting_value;
        list_of_ints.reserve(number_of_values);
        for (int val = starting_value; val <= ending_value; val++) {
            list_of_ints.emplace_back(DataValue(val));
        }
        return DataValue(list_of_ints);
    }

    Rule &number_maker;
    int ending_value;

    int starting_value;
};

// class InParallelRule : public Rule {
//     enum class StatementState { NOT_COMPLETED, COMPLETED };

// public:
//     InParallelRule(std::vector<Rule> &statements) {
//         for (auto &statement : statements) {
//             this->statements.emplace_back(statement, StatementState::NOT_COMPLETED);
//         }
//     }

// private:
//     void _handle_dependencies(NameResolver &name_resolver) override {
//         // Initialize all statements to be not completed
//         for (auto &statement : statements) {
//             statement.second = StatementState::NOT_COMPLETED;
//         }
//     }

//     DataValue _runBurst(NameResolver &name_resolver) override {
//         for (auto &statement_pair : statements) {
//             if (statement_pair.second == StatementState::COMPLETED) {
//                 continue;
//             }
//             // run the current statement, and check whether it finished
//             auto rule_state = statement_pair.first->runBurst(name_resolver);
//             if (rule_state.asRuleStatus() != DataValue::RuleStatus::NOTDONE) {
//                 statement_pair.second = StatementState::COMPLETED;
//             }
//         }
//         // check whether any statement are not completed, and if so, return NOTDONE
//         auto it = std::find_if(statements.begin(), statements.end(), [](const auto &statement_pair) {
//             return statement_pair.second == StatementState::NOT_COMPLETED;
//         });
//         if (it != statements.end()) {
//             return DataValue({DataValue::RuleStatus::NOTDONE});
//         }
//         return DataValue({DataValue::RuleStatus::DONE});
//     }

//     std::vector<std::pair<std::unique_ptr<Rule>, StatementState>> statements;
// };

class NameResolverRule : public Rule {
public:
    NameResolverRule(std::vector<std::string>& key) : search_keys(key) {}

private:
    void _handle_dependencies(NameResolver &name_resolver) override {
        if (search_keys.empty()) {
            std::cerr << "NameResolverRule called without arguments!" << std::endl;
        }
        isNested = search_keys.size() > 1;
    }

    DataValue _runBurst(NameResolver &name_resolver) override {
        auto valueInTopScope = name_resolver.getValue(search_keys[0]);
        if (valueInTopScope == std::nullopt) {
            return DataValue("ERROR");
        }
        
        if (isNested && valueInTopScope->getType() == "ORDERED_MAP") {
            auto valueInMap = name_resolver.getNestedValue(search_keys);    
            if (valueInMap == std::nullopt) {
                return DataValue("ERROR");
            }
            result = *valueInMap;
        } 
        else {
            result = *valueInTopScope;
        }

        return result;
    }

    // This vector has the individual components of the desired value in order,
    // separated by its periods (ie. configuration.setup -> ["configuration", "setup"]) )
    std::vector<std::string>& search_keys;
    bool isNested = false;
    DataValue result;
};