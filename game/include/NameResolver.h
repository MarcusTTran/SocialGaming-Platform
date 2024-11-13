#pragma once

#include <string>
#include <vector>
#include <unordered_map>

// TODO: use Ian's implementation
struct GameVariant
{
    std::string s_value;
    int i_value;
    void *v_value;
    Map m_value;
    std::vector<GameVariant> values;
    bool completed;
};

using Map = std::unordered_map<std::string, GameVariant>;

class NameResolver
{
public:
    void add_inner_scope() { full_scope.push_back({}); }
    void remove_inner_scope() { full_scope.pop_back(); }
    bool set_value(const std::string &key, const GameVariant &value)
    {
        for (auto it = full_scope.rbegin(); it != full_scope.rend(); ++it)
        {
            auto &map = *it;
            auto mapIt = map.find(key);

            if (mapIt != map.end())
            {
                mapIt->second = value;
                return true;
            }
        }
        return false;
    }
    GameVariant get_value(const std::string &key)
    {
        for (auto it : full_scope)
        {
            auto &map = it;
            auto mapIt = map.find(key);

            if (mapIt != map.end())
            {
                return mapIt->second;
            }
        }
        return {}; // null
    }
    bool add_new_value(const std::string &key, const GameVariant &value)
    {
        // return false if the key already exists in the inner-most scope
        if (full_scope.back().find(key) != full_scope.back().end())
        {
            return false;
        }
        // add new key/value pair to inner-most scope
        full_scope.back()[key] = value;
        return true;
    }

private:
    std::vector<Map> full_scope;
};
