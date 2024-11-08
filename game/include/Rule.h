#pragma once

#include <string>
#include <vector>
#include <cassert>
#include <unordered_map>

// TODO: use Ian's implementation
struct GameVariant
{
    std::string s_value;
    int i_value;
    void *v_value;
    Map m_value;
    std::vector<GameVariant> values;
    bool completed;
};

// TODO: use Nicholas's implementation
struct IO
{
    void send_message(Map recipient, std::string message);
};

// Example variables state
// vector([
//   { [configuration, {[name, "RPS"], [player_range, (1, 20)]}], [winners, vector(["Jake", "Mark"])] }, // Parse-time
//   { [rounds, 4] },                                                                                    // Setup-time
//   { [players, vector([...])] },                                                                       // Upon "Start Game"
//   { [round, 0] },                                                                                     // Execution-time - entering for
//   { [player, {[__id__, 23987562], [name, "Jake"], [wins, 0], [weapon, "Rock"]}] }                     // Execution-time - entering parallel-for
// ])

using Map = std::unordered_map<std::string, GameVariant>;

class NameResolver
{
public:
    void add_inner_scope() { full_scope.push_back({}); }
    void remove_inner_scope() { full_scope.pop_back(); }
    bool set_value(const std::string &key, const GameVariant &value)
    {
        for (auto it = full_scope.rbegin(); it != full_scope.rend(); ++it)
        {
            auto &map = *it;
            auto mapIt = map.find(key);

            if (mapIt != map.end())
            {
                mapIt->second = value;
                return true;
            }
        }
        return false;
    }
    GameVariant get_value(const std::string &key)
    {
        for (auto it : full_scope)
        {
            auto &map = it;
            auto mapIt = map.find(key);

            if (mapIt != map.end())
            {
                return mapIt->second;
            }
        }
        return {}; // null
    }
    bool add_new_value(const std::string &key, const GameVariant &value)
    {
        // return false if the key already exists in the inner-most scope
        if (full_scope.back().find(key) != full_scope.back().end())
        {
            return false;
        }
        // add new key/value pair to inner-most scope
        full_scope.back()[key] = value;
        return true;
    }

private:
    std::vector<Map> full_scope;
};

using Scope = Map;
using Variables = std::vector<Scope>;

class Rule
{
public:
    virtual ~Rule() = default;

    /*
     * Attempt to execute as much of the rule as possible.
     */
    GameVariant runBurst(NameResolver &name_resolver)
    {
        if (first_time)
        {
            _handle_dependencies(name_resolver);
        }
        first_time = false;
        name_resolver.add_inner_scope();
        GameVariant return_value = _runBurst(name_resolver);
        name_resolver.remove_inner_scope();
        if (return_value.completed)
        {
            first_time = true;
        }
        return return_value;
    }

private:
    /*
     * Evaluate execution-time parameters
     */
    virtual void _handle_dependencies(NameResolver &name_resolver) = 0;
    bool first_time = true;

    virtual GameVariant _runBurst(NameResolver &name_resolver) = 0;
};

class StringRule : public Rule
{
    StringRule(std::string string_literal)
        : string(string_literal) {}

private:
    void _handle_dependencies(NameResolver &name_resolver) override
    {
        // TODO: handle strings with {} braces
    }

    GameVariant _runBurst(NameResolver &name_resolver) override
    {
        GameVariant string_val;
        string_val.s_value = string;
        return string_val;
    }

    std::string string;
    std::vector<Rule &> dependencies;
};

class AllPlayersRule : public Rule
{
private:
    void _handle_dependencies(NameResolver &name_resolver) override {}

    GameVariant _runBurst(NameResolver &name_resolver) override
    {
        return name_resolver.get_value("players");
    }
};

class MessageRule : public Rule
{
    MessageRule(IO messager, Rule &recipient_list_maker, Rule &string_maker)
        : messager(messager), recipient_list_maker(recipient_list_maker), string_maker(string_maker) {};

private:
    void _handle_dependencies(NameResolver &name_resolver) override
    {
        auto recipients_generic = recipient_list_maker.runBurst(name_resolver);
        recipients = recipients_generic.values;

        auto message_generic = string_maker.runBurst(name_resolver);
        message = message_generic.s_value;
    }

    GameVariant _runBurst(NameResolver &name_resolver) override
    {
        for (auto person : recipients)
        {
            messager.send_message(person.m_value, message);
        }
        return {}; // void
    }

    IO messager;
    Rule &recipient_list_maker;
    Rule &string_maker;
    std::vector<GameVariant> recipients;
    std::string message;
};

class ForRule : public Rule
{
public:
    ForRule(std::string iterator_name, Rule &list_maker, std::vector<Rule> contents)
        : iterator_name{iterator_name}, list_maker{list_maker}, statement_list{contents} {}

    void _handle_dependencies(NameResolver &name_resolver) override
    {
        auto list_of_values_generic = list_maker.runBurst(name_resolver);
        list_of_values = list_of_values_generic.values;
        value_for_this_loop = list_of_values.begin();
        if (list_of_values.size() > 0)
        {
            name_resolver.add_new_value(iterator_name, (*value_for_this_loop));
        }

        current_statement = statement_list.begin();
    }

    GameVariant _runBurst(NameResolver &name_resolver) override
    {
        GameVariant return_value;
        if (list_of_values.size() < 1 || statement_list.size() < 1)
        {
            return_value.completed = true;
            return return_value; // complete
        }
        while (true)
        {
            assert(value_for_this_loop != list_of_values.end() && "The next iterator to run should always be valid");
            assert(current_statement != statement_list.end() && "The next statement to run should always be valid");
            // run the current sub-rule, and check whether it finished
            auto rule_state = (*current_statement).runBurst(name_resolver);
            if (!rule_state.completed)
            {
                return return_value; // incomplete
            }
            // set up next rule to run
            current_statement++;
            if (current_statement != statement_list.end())
            {
                continue;
            }
            // set up next full iteration
            current_statement = statement_list.begin();
            value_for_this_loop++;
            if (value_for_this_loop != list_of_values.end())
            {
                continue;
            }
            // every iteration complete
            return_value.completed = true;
            return return_value; // complete
        }
    }

private:
    std::string iterator_name;
    GameVariant iterator;

    Rule &list_maker;
    std::vector<GameVariant> list_of_values;
    std::vector<GameVariant>::iterator value_for_this_loop;

    std::vector<Rule> statement_list;
    std::vector<Rule>::iterator current_statement;
};
