#pragma once

#include "CommonVariantTypes.h"
#include <string>
#include <unordered_map>
#include <optional>

using Map = std::unordered_map<std::string, DataValue>;
// std::optional<DataValue> findInMap(const DataValue::OrderedMapType &map, const std::vector<std::string>& search_keys);

class NameResolver {
public:
    void addInnerScope() { full_scope.push_back({}); }
    void removeInnerScope() { full_scope.pop_back(); }

    bool addNewValue(const std::string &key, const DataValue &value) {
        // Create top level scope if empty
        if (full_scope.empty()) {
            full_scope.push_back({});
        }

        // return false if the key already exists in the inner-most scope
        if (full_scope.back().find(key) != full_scope.back().end()) {
            return false;
        }
        // add new key/value pair to inner-most scope
        full_scope.back()[key] = value;
        return true;
    }

    bool setValue(const std::string &key, const DataValue &value) {
        for (auto it = full_scope.rbegin(); it != full_scope.rend(); ++it) {
            auto &map = *it;
            auto mapIt = map.find(key);

            if (mapIt != map.end()) {
                mapIt->second = value;
                return true;
            }
        }
        return false;
    }
    // Returns a nullopt if value is not found.
    std::optional<DataValue> getValue(const std::string &key) {
        for (auto it = full_scope.rbegin(); it != full_scope.rend(); ++it) {
            const auto &map = *it;
            auto mapIt = map.find(key);

            if (mapIt != map.end()) {
                return mapIt->second;
            }
            else {
                auto result = searchOneLayerDeeper(map, key);
                if (result.has_value()) {
                    return result;
                }
            }
        } 
        return std::nullopt;
    }

    // Returns a nullopt if value is not found. Returns a DataValue from a nested structure of maps.
    std::optional<DataValue> getNestedValue(const std::vector<std::string>& search_keys) {
        auto valueInTopScope = getValue(search_keys[0]);
        if (valueInTopScope == std::nullopt) {
            return DataValue("ERROR");
        }
        
        std::vector<std::string> nestedKeys(search_keys);
        nestedKeys.erase(search_keys.begin());
        const auto& result = findInMap(valueInTopScope->asOrderedMap(), search_keys);
        return result;
    }

    // std::optional<DataValue> findInList(const std::vector<DataValue>& list, const std::vector<std::string>& search_keys) {
        
    //     // TODO: impl this for contains rule
    // }

private:
    // INVARIANT: key-value pair cannot be contained in any nested structure that is a list. 
    //            Must be made of maps only.
    std::optional<DataValue> findInMap(const DataValue::OrderedMapType &map, const std::vector<std::string>& search_keys) {
        const DataValue::OrderedMapType* current_map_level = &map;
        for (size_t i = 0; i < search_keys.size(); ++i) {
            // Keep searching through the levels of each map equal to the number of search key terms
            const auto& key = search_keys[i];
            auto it = current_map_level->find(key);
            
            // Did not find the next key:map pair when we have not reached the last level of nesting
            if (i < (search_keys.size() - 1) && it == current_map_level->end()) {
                return std::nullopt;
            }
            // Terminate loop if we found the key-value pair at the last level of nesting
            if (i == (search_keys.size() - 1) && it != current_map_level->end()) {
                return it->second;
            }

            // Otherwise continue searching
            current_map_level = &it->second.asOrderedMap();
        }

        // Should not reach this point!
        std::cerr << "Reach end of non-void function in findInMap()." << std::endl;
        return std::nullopt;
    }

    std::optional<DataValue> searchOneLayerDeeper(const Map& map, const std::string& key) {
        for (const auto& [innerKey, innerValue] : map) {
            if ( auto innerMap = std::get_if<DataValue::OrderedMapType>(&innerValue.getValue()) ) {
                auto innerMapIt = innerMap->find(key);
                if (innerMapIt != innerMap->end()) {
                    return innerMapIt->second;
                }
            }
        }
        return std::nullopt;
    }



    std::vector<Map> full_scope; 
};