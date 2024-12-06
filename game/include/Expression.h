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


// example: weapons.elements.name
// => vec {rock, paper, scissors}
// 
// takes 1) a list of maps map before the keyword ".elements." and 2) a string key after           
//      -> returns a flat list containing the values within those maps 
// INVARIANT: left operand is a Rule that MUST evaluate to a list of maps
class ElementsRule : public Rule {
public:
    ElementsRule(std::vector<std::unique_ptr<Rule>> maps_maker_rule, const std::string &key)
        : maps_maker{std::move(maps_maker_rule)}, search_key(key) {}

private:
    void _handle_dependencies(NameResolver &name_resolver) {
        current_map_maker = maps_maker.begin();
    }
    DataValue _runBurst(NameResolver &name_resolver) {
        while(current_map_maker != maps_maker.end()){
            map = (*current_map_maker)->runBurst(name_resolver).asOrderedMap();
            auto mapIt = map.find(this->search_key);
            if (mapIt!= map.end()) {
                //this only makes sense for this to be a list since multiple values can be contained with the same key
                assert(mapIt->second.getType() == "LIST" && "Expected map value to be a list");
                flattenedList.insert(flattenedList.end(), mapIt->second.asList().begin(), mapIt->second.asList().end());
            }
            current_map_maker++;
        }
        return DataValue(flattenedList);
    }
    DataValue::OrderedMapType map;
    std::vector<std::unique_ptr<Rule>>::iterator current_map_maker;
    std::vector<DataValue> flattenedList;
    std::vector<std::unique_ptr<Rule>> maps_maker; // NameResolver rule
    const std::string &search_key;
};

