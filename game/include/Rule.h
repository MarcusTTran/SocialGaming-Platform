#pragma once

#include <cassert>
#include <string>
#include <unordered_map>
#include <vector>

#include "CommonVariantTypes.h"
#include "Messenger.h"
#include "NameResolver.h"

// Example variables state
// vector([
//   { [configuration, {[name, "RPS"], [player_range, (1, 20)]}], [winners, vector(["Jake", "Mark"])] }, // Parse-time
//   { [rounds, 4] },                                                                                    // Setup-time
//   { [players, vector([...])] },                                                                       // Upon "Start
//   Game" { [round, 0] },                                                                                     //
//   Execution-time - entering for { [player, {[__id__, 23987562], [name, "Jake"], [wins, 0], [weapon, "Rock"]}] } //
//   Execution-time - entering parallel-for
// ])

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

// class StringRule : public Rule {
//     StringRule(std::string string_literal) : string(string_literal) {}

// private:
//     void _handle_dependencies(NameResolver &name_resolver) override {
//         // TODO: handle strings with {} braces
//     }

//     DataValue _runBurst(NameResolver &name_resolver) override {
//         DataValue string_val(string);
//         return string_val;
//     }

//     std::string string;
//     std::vector<Rule &> dependencies;
// };

// NOTE: Had to modify these two rules to have specific getters for the values they produce
// This is because the base rule class expects the return value to be a RuleStatus, but these rules
// return a string and a list of players, respectively. This is a temporary solution and should be
// replaced with a more general solution in the future.
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
        // TODO: handle error value returning
        return name_resolver.getValue("players");
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

// class ForRule : public Rule {
// public:
//     ForRule(std::string iterator_name, Rule &list_maker, std::vector<Rule> contents)
//         : iterator_name{iterator_name}, list_maker{list_maker}, statement_list{contents} {}

//     void _handle_dependencies(NameResolver &name_resolver) override {
//         auto list_of_values_generic = list_maker.runBurst(name_resolver);
//         list_of_values = list_of_values_generic.asList(); // TODO: figure out this error
//         value_for_this_loop = list_of_values.begin();
//         if (list_of_values.size() > 0) {
//             name_resolver.addNewValue(iterator_name, (*value_for_this_loop));
//         }

//         current_statement = statement_list.begin();
//     }

//     DataValue _runBurst(NameResolver &name_resolver) override {
//         DataValue return_value;
//         if (list_of_values.size() < 1 || statement_list.size() < 1) {
//             return_value = DataValue::RuleStatus::DONE;
//             return return_value; // complete
//         }
//         while (true) {
//             assert(value_for_this_loop != list_of_values.end() && "The next iterator to run should always be valid");
//             assert(current_statement != statement_list.end() && "The next statement to run should always be valid");
//             // run the current sub-rule, and check whether it finished
//             auto rule_state = (*current_statement).runBurst(name_resolver);
//             if (rule_state.asRuleStatus() == DataValue::RuleStatus::NOTDONE) {
//                 return return_value; // incomplete
//             }
//             // set up next rule to run
//             current_statement++;
//             if (current_statement != statement_list.end()) {
//                 continue;
//             }
//             // set up next full iteration
//             current_statement = statement_list.begin();
//             value_for_this_loop++;
//             if (value_for_this_loop != list_of_values.end()) {
//                 continue;
//             }
//             // every iteration complete
//             return_value = DataValue::RuleStatus::DONE;
//             return return_value; // complete
//         }
//     }

// private:
//     std::string iterator_name;
//     DataValue iterator;

//     Rule &list_maker;
//     std::vector<DataValue> list_of_values;
//     std::vector<DataValue>::iterator value_for_this_loop;

//     std::vector<Rule> statement_list;
//     std::vector<Rule>::iterator current_statement;
// };
class ForRule : public Rule {
public:
    ForRule(std::string iterator_name, Rule &list_maker, std::vector<std::unique_ptr<Rule>> contents)
        : iterator_name{std::move(iterator_name)}, list_maker{list_maker}, statement_list{std::move(contents)} {}

    void _handle_dependencies(NameResolver &name_resolver) override {
        auto list_of_values_generic = list_maker.runBurst(name_resolver);
        list_of_values = list_of_values_generic.asList(); // Ensure `asList` is implemented in `DataValue`
        value_for_this_loop = list_of_values.begin();

        if (!list_of_values.empty()) {
            name_resolver.addNewValue(iterator_name, *value_for_this_loop);
        }

        current_statement = statement_list.begin();
    }

    DataValue _runBurst(NameResolver &name_resolver) override {
        // Return type is DataValue; wrap RuleStatus appropriately
        if (list_of_values.empty() || statement_list.empty()) {
            return DataValue{DataValue::RuleStatus::DONE};
        }

        while (true) {
            assert(value_for_this_loop != list_of_values.end() && "Iterator for list_of_values is invalid");
            assert(current_statement != statement_list.end() && "Iterator for statement_list is invalid");

            // Run the current statement
            auto rule_state = (*current_statement)->runBurst(name_resolver); // Dereference unique_ptr
            if (rule_state.asRuleStatus() == DataValue::RuleStatus::NOTDONE) {
                return DataValue{DataValue::RuleStatus::NOTDONE}; // Incomplete
            }

            // Move to the next statement
            current_statement++;
            if (current_statement != statement_list.end()) {
                continue;
            }

            // If all statements are done, move to the next iteration
            current_statement = statement_list.begin();
            value_for_this_loop++;
            if (value_for_this_loop != list_of_values.end()) {
                name_resolver.addNewValue(iterator_name, *value_for_this_loop);
                continue;
            }

            // All iterations are complete
            return DataValue{DataValue::RuleStatus::DONE};
        }
    }

private:
    std::string iterator_name;
    DataValue iterator;

    Rule &list_maker;
    std::vector<DataValue> list_of_values;
    std::vector<DataValue>::iterator value_for_this_loop;

    std::vector<std::unique_ptr<Rule>> statement_list;
    std::vector<std::unique_ptr<Rule>>::iterator current_statement; // Correct iterator type
};