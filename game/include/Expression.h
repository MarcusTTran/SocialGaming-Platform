#pragma once
#include "CommonVariantTypes.h"
#include "Rule.h"
#include <string>
#include <cassert>

// TODO: add this to cmakeLists when ready

/* 
- Expressions are subsets of rules, thus they are derived from the Rule interface. 

- They may contain nested expressions inside of them as well.  For example, this 
  statement is an assignment operator with a nested + operator as an operand. 
        "winner.wins <- winner.wins + 1;"

- Expressions can be unary or binary, reffering to their number of arguments/operands.
*/



    // UNARY EXPRESSIONS RULES
// ex. list.size(), constants, logical !, 
// - usually evalutes to a primitive type like int or boolean, however, not sure 
//   if this is ALWAYS the case
// - operation is expressed by the name of the Rule (ie. ListSizeRule -> size() method)


class ListSizeRule : public Rule {
public:
    ListSizeRule(std::unique_ptr<Rule> operand_maker_rule)
        : operand_maker{std::move(operand_maker_rule)}  {}
    
private: 
    void _handle_dependencies(NameResolver &name_resolver){
        operand = operand_maker->runBurst(name_resolver);
    }    
    DataValue _runBurst(NameResolver &name_resolver){}
    
    std::unique_ptr<Rule> operand_maker; 
    DataValue operand; // will be a list
    DataValue result;
};


    // BINARY EXPRESSION RULES
// ex. +, -, *, /, assignment(x <- 5), ||, =(equality) 


// takes 1) a list of maps map before the keyword ".elements." and 2) a string key after           
//      -> returns a flat list containing the values within those maps 
// INVARIANT: left operand is a Rule that MUST evaluate to a list of maps
class ElementsRule : public Rule {
public:
    ElementsRule(std::unique_ptr<Rule> list_of_maps_rule, std::string key)
        : list_of_maps{std::move(list_of_maps_rule)}, search_key(key) {}

private:
    void _handle_dependencies(NameResolver &name_resolver) {}
    DataValue _runBurst(NameResolver &name_resolver) {}

    std::unique_ptr<Rule> list_of_maps; // NameResolver rule?
    std::string search_key;
    DataValue result; // DataValue::Type::LIST
};

