#pragma once
#include "CommonVariantTypes.h"
#include "Rule.h"
#include <string>

/* 
- Expressions are subsets of rules, thus they are derived from the Rule interface. 

- They may contain nested expressions inside of them as well.  For example, this 
  statement is an assignment operator with a nested + operator as an operand. 
        "winner.wins <- winner.wins + 1;"

*/


// ex. list.size(), constants, logical !, 
// - usually evalutes to a primitive type like int or boolean, however, not sure 
//   if this is ALWAYS the case
class UnaryExpressionRule : public Rule {
public:
    UnaryExpressionRule(std::unique_ptr<Rule> operand, std::string operation)
        : operand{std::move(operand)}, operation(operation) {}
    
private:
    void _handle_dependencies(NameResolver &name_resolver){}    
    DataValue _runBurst(NameResolver &name_resolver){}
    
    std::unique_ptr<Rule> operand; // This could be a boolean, int, list, ...
    std::string operation; // examples: .size(), !
    DataValue result;
};

// ex. +, -, *, /, assignment(x <- 5), 
// .elements() since it takes 1) a map before and 2) a key after -> returns a list
class BinaryExpressionRule : public Rule {
public:
    

private:
    void _handle_dependencies(NameResolver &name_resolver) {}
    DataValue _runBurst(NameResolver &name_resolver) {}

    std::unique_ptr<Rule> left_operand;
    std::unique_ptr<Rule> right_operand;
    std::string operation;
    DataValue result; // could be list, int, bool, etc.
};







// TODO: potentially add a +ary nested expression type
/*
    For example, this assignment is an assignment operator, but also has a + operator
        - "winner.wins <- winner.wins + 1;"
    

*/

