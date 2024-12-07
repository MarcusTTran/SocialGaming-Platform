#pragma once
#include "CommonVariantTypes.h"
#include "Rule.h"
#include <string>
#include <cassert>
#include <type_traits>
#include <algorithm>

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
    ListSizeRule(std::unique_ptr<Rule> list_maker)
        : list_maker{std::move(list_maker)}  {}
    
private: 
    void _handle_dependencies(NameResolver &name_resolver){
        list = list_maker->runBurst(name_resolver);
    }    
    DataValue _runBurst(NameResolver &name_resolver){
        size_t size = list.asList().size();
        return DataValue(static_cast<int>(size));
    }
    
    std::unique_ptr<Rule> list_maker; 
    DataValue list; 
};


    // BINARY EXPRESSION RULES
// ex. +, -, *, /, assignment(x <- 5), ||, =(equality) 


// takes 1) a list of maps map before the keyword ".elements." and 2) a string key after           
//      -> returns a flat list containing the values within those maps 
// INVARIANT: left operand is a Rule that MUST evaluate to a list of maps
// class ElementsRule : public Rule {
// public:
//     ElementsRule(std::unique_ptr<Rule> list_of_maps_rule, std::string key)
//         : list_of_maps{std::move(list_of_maps_rule)}, search_key(key) {}

// private:
//     void _handle_dependencies(NameResolver &name_resolver) {}
//     DataValue _runBurst(NameResolver &name_resolver) {}

//     std::unique_ptr<Rule> list_of_maps; // NameResolver rule?
//     std::string search_key;
//     DataValue result; // DataValue::Type::LIST
// };







/*
COLLECT RULE

for weapon in weapons {
    match !players.elements.weapon.contains(weapon.name) {
        true => {
            extend winners with players.collect(player, player.weapon = weapon.beats);
        }
    }
}

- 0th variable: list of DataValues from left-hand side -> NameResolverRule
- first variable: fresh variable from left-hand side of collect() -> string
- second variable: lambda that evaluates to true or false -> TODO: = equals rule and NameResolverRule on both sides

*/




/*
EQUALS RULE

- INVARIANT: arguments are not collections, but singular values (like int, bool, string)
*/ 
class EqualsRule : public Rule {
public:
    EqualsRule(std::unique_ptr<Rule> left_maker, std::unique_ptr<Rule> right_maker)
        : left_maker{std::move(left_maker)}, right_maker{std::move(right_maker)} {}

private:
    void _handle_dependencies(NameResolver &name_resolver) override {
        left = left_maker->runBurst(name_resolver);
        right = right_maker->runBurst(name_resolver);   
    }

    DataValue _runBurst(NameResolver &name_resolver) override {
        return DataValue(left.checkIfMatch(right));
    }

    std::unique_ptr<Rule> left_maker;
    std::unique_ptr<Rule> right_maker;
    DataValue left;
    DataValue right;
};


/*
EXTEND RULE

- with is a keyword!
- syntax:    extend <var1> with <var2>
- 1st variable: list that you are adding to -> NameResolverRule
- 2nd variable: list that you are appending -> in this case, collect() rule

- INVARIANT: Lists must be of the same type. Cannot extend a list of type "x" with a list of type "y"
*/
class ExtendRule : public Rule {
public:
    ExtendRule(std::unique_ptr<Rule> original_list_maker, std::unique_ptr<Rule> additional_list_maker)
        : original_list_maker{std::move(original_list_maker)}, additional_list_maker{std::move(additional_list_maker)} {}

private:    
    
    void _handle_dependencies(NameResolver &name_resolver) override {
        listToExtend = original_list_maker->runBurst(name_resolver);
        listToAdd = additional_list_maker->runBurst(name_resolver);   
    }

    DataValue _runBurst(NameResolver &name_resolver) override {
        // Check if the lists are of same type
        if( !(listToExtend.getType() == "LIST") || !(listToAdd.getType() == "LIST") ){
            std::cerr << "ERROR ExtendRule: Both arguments must be lists!" << std::endl;
            return DataValue( {DataValue::RuleStatus::ERROR} );
        }
        // Reserve space for extended list
        auto& listToExtendAsVec = listToExtend.asList();
        auto& listToAddAsVec = listToAdd.asList();
        listToExtendAsVec.reserve(listToExtendAsVec.size() + listToAddAsVec.size());

        std::copy(listToAddAsVec.begin(), listToAddAsVec.end(), listToExtendAsVec.back());
    }
    
    std::unique_ptr<Rule> original_list_maker;
    std::unique_ptr<Rule> additional_list_maker;
    DataValue listToExtend;
    DataValue listToAdd;
};