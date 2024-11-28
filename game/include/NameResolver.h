#pragma once

#include "CommonVariantTypes.h"
#include <string>
#include <unordered_map>
#include <optional>

using Map = std::unordered_map<std::string, DataValue>;

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
    // Returns a nullopt if value is not found. You can use the .hasvalue() method.
    std::optional<DataValue> getValue(const std::string &key) {
        for (auto it = full_scope.rbegin(); it != full_scope.rend(); ++it) {
            auto &map = *it;
            auto mapIt = map.find(key);

            if (mapIt != map.end()) {
                return mapIt->second;
            }
        }
        return std::nullopt;
    }

    // INVARIANT: key-value pair cannot be contained in any nested structure that is a list. 
    //            Must be made of maps only.
    static std::optional<DataValue> findInMap(const Map &map, const std::vector<std::string>& search_keys) {
        // TODO: make this search through nested maps within a map?
        Map current_map_level = map;
        for (const auto &key : search_keys) {
            // Keep searching through the levels of each map equal to the number of search key terms
        }
       
    }

private:
    std::vector<Map> full_scope; 
