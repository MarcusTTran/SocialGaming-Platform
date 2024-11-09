#pragma once

#include <cassert>
#include <string>
#include <vector>

// TODO: make a variable class
struct Variable {
  std::string name;
  int value;
};

// TODO: make a class that represents a bundle of variables
using Variables = std::vector<Variable>;

class Rule {
public:
  enum RuleState { Completed, Incomplete };

  virtual ~Rule() = default;

  /*
   * Set or reset the rule state to be run as if never ran before.
   */
  virtual void set() = 0;

  /*
   * Attempt to execute as much of the rule as possible.
   */
  virtual RuleState runBurst(Variables &context) = 0;
};

// class MessageRule : public Rule {
//   MessageRule() {};

//   RuleState runBurst() override {
//     for (auto person : recipients) {
//       message(person, message);
//     }
//   }
// };

template <typename T> class ForRule : public Rule {
public:
  ForRule(std::string iterator_name, Parsable list_expression, std::vector<Rule> contents)
      : list_of_values{list}, iterator_name{iterator_name}, statement_list{contents} {
    reset();
  }

  void set() override {
    list_of_values = list_expression.parse(context);
    value_for_this_loop = list_of_values.begin();
    current_statement = statement_list.begin();
    if (list_of_values.size() > 0) {
      iterator = Variable{iterator_name, (*value_for_this_loop).value};
    }
  }

  RuleState runBurst(Variables &context) override {
    // First time must run set()
    if (!is_mid_run) {
      set();
      is_mid_run = true;
    }
    if (list_of_values.size() < 1 || statement_list.size() < 1) {
      return Rule::Completed;
    }
    while (true) {
      assert(value_for_this_loop != list_of_values.end() && "The next iterator to run should always be valid");
      assert(current_statement != statement_list.end() && "The next statement to run should always be valid");
      // run the current sub-rule, and check whether it finished
      // TODO: Pass in the loop iterator Variable along with the external context
      //       and handle switching and removing the value as necessary
      auto rule_state = (*current_statement).runBurst(context);
      if (rule_state == Rule::Incomplete) {
        return Rule::Incomplete;
      }
      // set up next rule to run
      current_statement++;
      if (current_statement != statement_list.end()) {
        continue;
      }
      // set up next full iteration
      current_statement = statement_list.begin();
      value_for_this_loop++;
      if (value_for_this_loop != list_of_values.end()) {
        continue;
      }
      // every iteration complete
      is_mid_run = false;
      return Rule::Completed;
    }
  }

private:
  bool is_mid_run = false;

  std::string iterator_name;
  Variable iterator;

  std::vector<T> list_of_values;
  Variables::iterator value_for_this_loop;

  std::vector<Rule> statement_list;
  std::vector<Rule>::iterator current_statement;
};
