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
        if (first_time) {
            _handle_dependencies(name_resolver);
        }
        first_time = false;
        name_resolver.addInnerScope();
        DataValue returnValue = _runBurst(name_resolver);
        name_resolver.removeInnerScope();
        if (returnValue.asRuleStatus() == DataValue::RuleStatus::DONE) {
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
    DataValue _runBurst(NameResolver &name_resolver) override {
        return DataValue(number);
    }
    const int number;
};

class BooleanRule : public Rule {
public:
    BooleanRule(bool boolean) : boolean(boolean) {}
private:
    void _handle_dependencies(NameResolver &name_resolver) override {}
    DataValue _runBurst(NameResolver &name_resolver) override {
        return DataValue(boolean);
    }
    const bool boolean;
};

class StringRule : public Rule {
    StringRule(std::string string_literal) : string(string_literal) {}

private:
    void _handle_dependencies(NameResolver &name_resolver) override {
        // TODO: handle strings with {} braces
    }

    DataValue _runBurst(NameResolver &name_resolver) override {
        DataValue string_val(string);
        return string_val;
    }

    std::string string;
    std::vector<Rule &> dependencies;
};

class AllPlayersRule : public Rule {
private:
    void _handle_dependencies(NameResolver &name_resolver) override {}

    DataValue _runBurst(NameResolver &name_resolver) override {
        // TODO: handle error value returning
        return name_resolver.getValue("players");
    }
};

class MessageRule : public Rule {
    MessageRule(std::shared_ptr<IServer> server, Rule &recipient_list_maker, Rule &string_maker)
        : messager(server), recipient_list_maker(recipient_list_maker), string_maker(string_maker) {};

private:
    void _handle_dependencies(NameResolver &name_resolver) override {
        auto recipients_generic = recipient_list_maker.runBurst(name_resolver);
        recipients = recipients_generic.asList();

        auto message_generic = string_maker.runBurst(name_resolver);
        message = message_generic.asString();
    }

    DataValue _runBurst(NameResolver &name_resolver) override {
        for (auto person : recipients) {
            messager->sendMessageToPlayerMap(message, person.asOrderedMap());
        }
        return {}; // void TODO: change to error return type
    }

    std::shared_ptr<IServer> messager;
    Rule &recipient_list_maker;
    Rule &string_maker;
    std::vector<DataValue> recipients;
    std::string message;
};

class ForRule : public Rule {
public:
    ForRule(std::string fresh_variable_name, Rule &list_maker, std::vector<Rule> contents)
        : fresh_variable_name{fresh_variable_name}, list_maker{list_maker}, statement_list{contents} {}

private:
    void _handle_dependencies(NameResolver &name_resolver) override {
        // Run the rule that provides a list of values
        auto list_of_values_generic = list_maker.runBurst(name_resolver);
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

        // Run 
        while (true) {
            assert(value_for_this_loop != list_of_values.end() && "The next iterator to run should always be valid");
            assert(current_statement != statement_list.end() && "The next statement to run should always be valid");
            // run the current sub-rule, and check whether it finished
            auto rule_state = (*current_statement).runBurst(name_resolver);
            if (rule_state.asRuleStatus() == DataValue::RuleStatus::NOTDONE) {
                return DataValue(rule_state);
            }
            // set up next rule to run
            current_statement++;
            if (current_statement != statement_list.end()) {
                continue;
            }
            current_statement = statement_list.begin();
            // set up next full iteration
            value_for_this_loop++;
            if (value_for_this_loop != list_of_values.end()) {
                name_resolver.setValue(fresh_variable_name, *value_for_this_loop);
                continue;
            }
            // every iteration complete
            return DataValue({DataValue::RuleStatus::DONE});
        }
    }

    std::string fresh_variable_name;

    Rule &list_maker;
    std::vector<DataValue> list_of_values;
    std::vector<DataValue>::iterator value_for_this_loop;

    std::vector<Rule> statement_list;
    std::vector<Rule>::iterator current_statement;
};

class UpfromRule : public Rule {
public:
    UpfromRule(Rule &number_maker, int starting_value)
        : number_maker(number_maker), starting_value(starting_value) {}

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
