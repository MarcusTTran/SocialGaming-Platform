#pragma once

#include <string>
#include <vector>
#include <cassert>

// TODO: make a variable class
struct Variable
{
    std::string name;
    int value;
};

// TODO: make a class that represents a bundle of variables
using Variables = std::vector<Variable>;

class Rule
{
public:
    enum RuleState
    {
        Completed,
        Incomplete
    };

    virtual ~Rule() = default;

    /*
     * Attempt to execute as much of the rule as possible.
     */
    virtual RuleState runBurst() = 0;
};

class ForRule : public Rule
{
public:
    ForRule(Variables &context, std::string iterator_name, Variables list, std::vector<Rule> contents)
        : variables_in_scope{context}, list_of_values{list}, iterator_name{iterator_name}, statement_list{contents}
    {
        value_for_this_loop = list_of_values.begin();
        current_statement = statement_list.begin();
    }

    RuleState runBurst() override
    {
        if (list_of_values.size() < 1 || statement_list.size() < 1)
        {
            return Rule::Completed;
        }
        while (true)
        {
            assert(value_for_this_loop != list_of_values.end() && "The next iterator to run should always be valid");
            assert(current_statement != statement_list.end() && "The next statement to run should always be valid");
            // run the current sub-rule, and check whether it finished
            auto rule_state = (*current_statement).runBurst();
            if (rule_state == Rule::Incomplete)
            {
                return Rule::Incomplete;
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
            return Rule::Completed;
        }
    }

private:
    Variables variables_in_scope;
    Variables list_of_values;
    Variables::iterator value_for_this_loop;
    std::string iterator_name;

    std::vector<Rule> statement_list;
    std::vector<Rule>::iterator current_statement;
};
