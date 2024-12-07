#pragma once
#include "CommonVariantTypes.h"
#include "Rule.h"
#include <string>
#include <cassert>
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




/*
COLLECT RULE
written by: dma122, mtt8

for weapon in weapons {
    match !players.elements.weapon.contains(weapon.name) {
        true => {
            extend winners with players.collect(player, player.weapon = weapon.beats);
        }
    }
}

- 0th variable: list of DataValues from left-hand side -> NameResolverRule
- first variable: fresh variable from left-hand side of collect(). It is the thing we are collecting -> string
- second variable: lambda that evaluates to true or false -> = equals rule and NameResolverRule on both sides

winners [{name: Marcus .....}, {name:David ....}  + extend ... ]
players [{name: Marcus .....}, {name:David ....}, ... ]

winners.elements.name -> 

*/

// class CollectRule : public Rule {
// public:
//     CollectRule(std::string fresh_variable, std::string left_operand_key, std::unique_ptr<Rule> right_operand_maker, std::unique_ptr<Rule> list_maker)
//         : fresh_variable{fresh_variable}, left_operand_maker{std::move(left_operand_maker)}, right_operand_maker{std::move(right_operand_maker)}, 
//             list_maker(std::move(list_maker)) {}

// private:
//     void _handle_dependencies(NameResolver &name_resolver) {
//         //name resolver will find the list of player maps for us
//         collected_maps = const_cast< std::vector<DataValue>& > (list_maker->runBurst(name_resolver).asList()); 
//         left_operand = left_operand_maker->runBurst(name_resolver); 
//         right_operand = right_operand_maker->runBurst(name_resolver);
//     }    

    
//     DataValue _runBurst(NameResolver &name_resolver) { 
//         current_map = collected_maps.begin();
        
//         while (current_map != collected_maps.end()){
//             map = (*current_map)->runBurst(name_resolver);
//             // left_operand = current_map.find(weapon)
//             // compare (left_operand, right_operand)
//             // if true, add to winners


//             current_map++;   
//         }
//         return left_operand;
//     }   



//     std::string left_operand_key;
//     std::unique_ptr<Rule> right_operand_maker;
//     std::unique_ptr<Rule> equality_rule; // TODO: construct equality rule as we go

//     //List of individual player maps that we need to return
//     std::vector<DataValue> collected_maps;
//     // DataValue collected_maps;
//     std::vector<DataValue>::iterator current_map;
//     // DataValue::OrderedMapType map;
//     std::string fresh_variable; // What we want to collect into a list (ex. "Player" maps)
//     std::unique_ptr<Rule> list_maker; // NameResolver
//     DataValue left_operand; // String
//     DataValue right_operand; // String
// };



/*
EQUALS RULE
written by: jrspence

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
        auto& listToExtendAsVec = const_cast< std::vector<DataValue>& >(listToExtend.asList());
        auto& listToAddAsVec = listToAdd.asList();
        listToExtendAsVec.reserve(listToExtendAsVec.size() + listToAddAsVec.size());

        std::copy(listToAddAsVec.begin(), listToAddAsVec.end(), std::back_inserter(listToExtendAsVec) );
        return DataValue(listToExtendAsVec);
    }
    
    std::unique_ptr<Rule> original_list_maker;
    std::unique_ptr<Rule> additional_list_maker;
    DataValue listToExtend;
    DataValue listToAdd;
};


// example: weapons.elements.name
// => vec {rock, paper, scissors}
// 
// takes 1) a list of maps map before the keyword ".elements." and 2) a string key after           
//      -> returns a flat list containing the values within those maps 
// INVARIANT: left operand is a Rule that MUST evaluate to a list of maps
class ElementsRule : public Rule {
public:
    ElementsRule(std::unique_ptr<Rule> map_maker_rule, const std::string &key)
        : map_maker{std::move(map_maker_rule)}, search_key(key) {}

private:
    void _handle_dependencies(NameResolver &name_resolver) {
        map = map_maker->runBurst(name_resolver).asOrderedMap();
    }
    DataValue _runBurst(NameResolver &name_resolver) {
        auto mapIt = map.find(this->search_key);
        if (mapIt!= map.end()) { //this only makes sense for this to be a list since multiple values can be contained with the same key
            assert(mapIt->second.getType() == "LIST" && "Expected map value to be a list");
            flattenedList.insert(flattenedList.end(), mapIt->second.asList().begin(), mapIt->second.asList().end());
            return DataValue(flattenedList);
        }
        else if (mapIt->second.getType() == "ORDERED_MAP"){ // does not contain the key in this layer, check one layer deeper
            auto secondLayer = mapIt->second.asOrderedMap().find(this->search_key);
            if (secondLayer != mapIt->second.asOrderedMap().end()) {
                assert(secondLayer->second.getType() == "LIST" && "Expected map value to be a list");
                flattenedList.insert(flattenedList.end(), secondLayer->second.asList().begin(), secondLayer->second.asList().end());
                return DataValue(flattenedList);
            }
        }
        else{
            std::cout<<"ElementsRule was unable to find the list corresponding to: " << search_key <<std::endl;
        }
    }
    DataValue::OrderedMapType map;
    std::vector<DataValue> flattenedList;
    std::unique_ptr<Rule> map_maker; // NameResolver rule
    const std::string &search_key;
};

