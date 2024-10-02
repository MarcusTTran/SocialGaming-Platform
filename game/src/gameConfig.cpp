// author: kwa132, Mtt8

#include "gameConfig.h"
#include "algorithm"
#include <iostream>
#include <cassert>
#include <sstream>

extern "C" {
TSLanguage* tree_sitter_json();
}

GameConfig::GameConfig(const string& config){
    parseConfig(config);
};

string GameConfig::getGameName() const{
    return gameName;
}

pair<int, int> GameConfig::getPlayerRange() const{
    return playerRange;
}

bool GameConfig::hasAudience() const{
    return audience;
}
    
map<string, string> GameConfig::getConstants(){
    return constants;
}

map<string, string> GameConfig::getVariables(){
    return variables;
}

void GameConfig::gameNameSetter(const string& name){
    gameName = name;
}

void GameConfig::rangeSetter(const pair<int, int>& range){
    playerRange = range;
}

void GameConfig::audienceSetter(bool b){
    audience = b;
}

void GameConfig::constantsSetter(const string& key, const string& value){
    constants[key] = value;
}

void GameConfig::variablesSetter(const string& key, const string& value){
    constants[key] = value;
}

std::pair<std::string, std::string> GameConfig::parserHelper(const ts::Node& node, size_t i, const std::string& source) {
    ts::Node pairN = node.getNamedChild(static_cast<uint32_t>(i));
    ts::Node key = pairN.getChildByFieldName("key");
    ts::Node value = pairN.getChildByFieldName("value");
    auto keyRange = key.getByteRange();
    auto valueRange = value.getByteRange();

    std::string keyText = source.substr(keyRange.start, keyRange.end - keyRange.start);
    std::string valueText = source.substr(valueRange.start, valueRange.end - valueRange.start);
    return {keyText, valueText};
}

/*
    key:value
    configuration {
        name: "Rock-Paper-Scissors"
        player range: (2, 4)
        audience: false
    }
*/
void GameConfig::parseConfigurationSection(const ts::Node& node, const string& source){
    auto size = node.getNumNamedChildren();
    for(size_t i = 0; i < size; ++i){
        auto curr = parserHelper(node, i, source);
        string keyText = curr.first;
        string valueText = curr.second;
        
        if(keyText == "name"){
            gameNameSetter(valueText);
        }
        else if(keyText == "player range"){
            string_view input(valueText);
            input.remove_prefix(1);
            input.remove_suffix(1);
            auto comma = input.find(",");
            auto firstNumS = input.substr(0, comma);
            auto secondNumS = input.substr(comma + 2);
            auto firstNum = stoi(string(firstNumS));
            auto secondNum = stoi(string(secondNumS));
            rangeSetter({firstNum, secondNum});
        }
        else if(keyText == "audience"){
            valueText == "true" ? audienceSetter(true) 
                : audienceSetter(false);
        }
    }
}

/*
    {
        "weapons": "[ { name: \"Rock\", beats: \"Scissors\" }, 
            { name: \"Paper\", beats: \"Rock\" }, 
            { name: \"Scissors\", beats: \"Paper\" } ]",
        "max_rounds": "10",
        "timer_enabled": "false"
    }
*/

void GameConfig::parseConstantsSection(const ts::Node& node, const string& source){
    auto size = node.getNumNamedChildren();
    for(size_t i = 0; i < size; ++i){
        auto curr = parserHelper(node, i, source);
        string keyText = curr.first;
        string valueText = curr.second;
        
        if (keyText == "weapons") {
            parseWeapons(valueText);
        } else {
            constantsSetter(keyText, valueText);
        }
    }
}

void GameConfig::parseVariablesSection(const ts::Node& node, const string& source){
    auto size = node.getNumNamedChildren();
    for(size_t i = 0; i < size; ++i){
        auto curr = parserHelper(node, i, source);
        string keyText = curr.first;
        string valueText = curr.second;
        
        variablesSetter(keyText, valueText);
    }
}
    
void GameConfig::parseConfig(const string& config){
    ts::Language language = tree_sitter_json();
    ts::Parser parser{language};
    ts::Tree tree = parser.parseString(config);

    ts::Node root = tree.getRootNode();
    
    cout << "Tree: " << root.getSExpr().get() << endl;
    auto size = root.getNumNamedChildren();
    for(size_t i = 0; i < size; ++i){
        ts::Node curr = root.getNamedChild(i);
        string currType = string(curr.getType());
        
        if(currType == "configuration"){
            parseConfigurationSection(curr, config);
        }
        else if(currType == "constants"){
            parseConstantsSection(curr, config);
        }
        else if(currType == "variables"){
            parseVariablesSection(curr, config);
        }
    }
}
    
map<string, string> GameConfig::getRules() const{
    map<string, string> rules;
    for (const auto& [weapon, beats] : weaponsRules) {
        rules[weapon] = beats;
    }
    return rules;
}

void GameConfig::parseWeapons(const string& value){
    stringstream input(value);
    string weapon, beats;

    // Simple parsing logic for demonstration (better to use a JSON parser if format changes)
    while (input >> weapon >> beats) {
        weapon = weapon.substr(weapon.find(":") + 2, weapon.find(",") - weapon.find(":") - 2);
        beats = beats.substr(beats.find(":") + 2, beats.find("}") - beats.find(":") - 2);
        weaponsRules.emplace(weapon, beats);
    }   
}

void GameConfig::displayGameConfig(){
    cout << "Game Name: " << getGameName() << endl;
    cout << "Player range: " << "(" << getPlayerRange().first << ", " << 
    getPlayerRange().second << ")" << endl;
    cout << "Allowing audience: " << (hasAudience() ? "Yes" : "No") << endl;

    auto constants = getConstants();
    cout << "Constants:" << endl;
    for (const auto& [key, value] : constants) {
        cout << key << ": " << value << endl;
    }
    auto variables = getVariables();
    cout << "Variables:" << endl;
    for (const auto& [key, value] : variables) {
        cout << key << ": " << value << endl;
    }
}