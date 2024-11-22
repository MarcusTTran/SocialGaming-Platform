#pragma once

#include "CommonVariantTypes.h"
#include <string>
#include <unordered_map>

using Map = std::unordered_map<std::string, DataValue>;

class NameResolver {
public:
    void addInnerScope() { full_scope.push_back({}); }
    void removeInnerScope() { full_scope.pop_back(); }

    bool addNewValue(const std::string &key, const DataValue &value) {

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
    DataValue getValue(const std::string &key) {
        for (auto it : full_scope) {
            auto &map = it;
            auto mapIt = map.find(key);

            if (mapIt != map.end()) {
                return mapIt->second;
            }
        }
        return {}; // null TODO: change to error value
    }

private:
    std::vector<Map> full_scope;
};
