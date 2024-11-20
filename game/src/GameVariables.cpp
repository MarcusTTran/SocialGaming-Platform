#include "GameVariables.h"

GameVariables::GameVariables(const ParsedGameData &parserObject) : variables(parserObject.getVariables()) {};

auto GameVariables::findVariable(const std::string &key) const {
    return std::find_if(variables.begin(), variables.end(),
                        [&key](const std::pair<std::string, DataValue> &entry) { return entry.first == key; });
}

DataValue::OrderedMapType GameVariables::getVariables() const { return variables; }

auto GameVariables::findVariable(const std::string &key) {
    return std::find_if(variables.begin(), variables.end(),
                        [&key](const std::pair<std::string, DataValue> &entry) { return entry.first == key; });
}

const DataValue &GameVariables::getVariable(const std::string &key) const {
    auto valueFinder = findVariable(key);
    if (valueFinder != end(variables)) {
        return valueFinder->second;
    }
    static const DataValue emptyDataValue;
    return emptyDataValue;
}

void GameVariables::setVariable(const std::string &key, DataValue content) {
    auto valueFinder = findVariable(key);
    if (valueFinder != end(variables)) {
        valueFinder->second = std::move(content);
    } else {
        variables.emplace_back(key, std::move(content));
    }
}