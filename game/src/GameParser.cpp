// author: kwa132, Mtt8

#include "GameParser.h"

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

map<string, string> GameConfig::getConfiguration(){
    return configuration;
}

map<string, vector<map<string, string>>> GameConfig::getSetup(){
    return setup;
}

map<string, vector<pair<pair<string, string>, pair<string, string>>>> GameConfig::getConstants(){
    return constants;
}

map<string, string> GameConfig::getVariables(){
    return variables;
}

map<string, string> GameConfig::getPerPlayer(){
    return perPlayer;
}

map<string, string> GameConfig::getPerAudience(){
    return perAudience;
}

string GameConfig::extractStringValue(const ts::Node& node, const string& source){
    if (node.getType() == "quoted_string") {
        ts::Node textNode = node.getNamedChild(0);  
        if (!textNode.isNull() && textNode.getType() == "string_text") {
            return string(textNode.getSourceRange(source));  
        }
    }
    for(auto child : ts::Children{node}){
        auto type = string(child.getType());
        if(find(begin(toSkip), end(toSkip), type) != end(toSkip)){
            continue;
        }
        extractStringValue(child, source);
    }
    return string(node.getSourceRange(source));    
}
void GameConfig::extractStringValue2(const ts::Node& node, const std::string& source, pair<string, string> &str1, pair<string, string> &str2, std::string keyID){
    if(!node.getNumNamedChildren()){
        auto type = string(node.getType());
        if(find(begin(toSkip), end(toSkip), type) == end(toSkip)){
            auto curr = string(node.getSourceRange(source));
            str1.first.empty() ? str1.first = curr : (str1.second.empty() ? str1.second = curr : 
            (str2.first.empty() ? str2.first = curr : str2.second = curr));
        }
        return;
    }

    for(auto child : ts::Children{node}){
        auto type = string(child.getType());
        if(find(begin(toSkip), end(toSkip), type) != end(toSkip)){
            continue;
        }
        extractStringValue2(child, source, str1, str2, keyID);
    }
    if(!str1.first.empty() && !str1.second.empty() && !str2.first.empty() && !str2.second.empty()){
        constants[keyID].push_back({str1, str2});
        str1 = {};
        str2 = {};
    }    
}
template <typename T>
void GameConfig::parseValueMap(const ts::Node& node, const string& source, T& outputMap){
    for (size_t i = 0; i < node.getNumNamedChildren(); ++i) {
        ts::Node entryNode = node.getNamedChild(i);
        if (entryNode.getType() == "map_entry") {
            ts::Node keyNode = entryNode.getChildByFieldName("key");
            ts::Node valueNode = entryNode.getChildByFieldName("value");
            if (!keyNode.isNull() && !valueNode.isNull()) {
                string key = string(keyNode.getSourceRange(source));
                pair<string, string> str1;
                pair<string, string> str2;
                str1.first = "", str1.second = "";
                str2.first = "", str2.second = "";
                extractStringValue2(valueNode, source, str1, str2, key);   
            }
        }
    }    
}
void GameConfig::setupHelper(const ts::Node& node, const string& source, string& str1, string& str2, const string& keyID){
    auto type = string(node.getType());
    if (node.getType() == "number_range") {
        string rangeStr;
        for (const auto& child : ts::Children{node}) {
            if (child.getType() == "number") {
                string res = string(child.getSourceRange(source));
                rangeStr.empty() ? rangeStr = res : rangeStr += ", " + res;
            }
        }

        str2 = rangeStr;
        map<string, string> tempMap;
        tempMap[str1] = str2;
        setup[keyID].push_back(tempMap);

        str1.clear();
        str2.clear();
        return;  
    }

    if (node.getNumChildren() == 0 && (find(begin(toSkip), end(toSkip), type) == end(toSkip))) {
    std::string res = string(node.getSourceRange(source));
        if (!res.empty()) {
            size_t pos = res.find(":");
            if (pos != std::string::npos && pos == res.size() - 1) {
                res.erase(pos);  
            }
        }

    str1.empty() ? str1 = res : str2 = res;
}

    for (const auto& child : ts::Children{node}) {
        setupHelper(child, source, str1, str2, keyID);
    }

    if (!str1.empty() && !str2.empty()) {
        map<string, string> tempMap;
        tempMap[str1] = str2;
        setup[keyID].push_back(tempMap);

        str1.clear();
        str2.clear();
    }    
}
void GameConfig::parseMapHelper(const ts::Node& node, const string& source, map<string, string>& output){
    ts::Node mapNode = node.getChildByFieldName("map");

    if (!mapNode.isNull()) {
        for (size_t i = 0; i < mapNode.getNumNamedChildren(); ++i) {
            ts::Node entryNode = mapNode.getNamedChild(i);
            if (entryNode.getType() == "map_entry") {
                ts::Node keyNode = entryNode.getChildByFieldName("key");
                ts::Node valueNode = entryNode.getChildByFieldName("value");
                if (!keyNode.isNull() && !valueNode.isNull()) {
                    string key = string(keyNode.getSourceRange(source));
                    string value = string(valueNode.getSourceRange(source));
                    output[key] = value;
                }
            }
        }
    }      
}
void GameConfig::parsePerPlayerSection(const ts::Node& node, const string& source){
    parseMapHelper(node, source, perPlayer);
}
void GameConfig::parsePerAudienceSection(const ts::Node& node, const string& source){
    parseMapHelper(node, source, perAudience);    
}
void GameConfig::printTree(const ts::Node& node, const string& source, int indent = 0){
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << node.getType() << " -> " << node.getSourceRange(source) << std::endl;

    for (size_t i = 0; i < node.getNumNamedChildren(); ++i) {
        printTree(node.getNamedChild(i), source, indent + 1);
    }    
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
    ts::Node nameNode = node.getChildByFieldName("name");
    if (!nameNode.isNull()) {
        configuration["name"] = extractStringValue(nameNode, source);
    }

    ts::Node playerRangeNode = node.getChildByFieldName("player_range");
    if (!playerRangeNode.isNull()) {
        configuration["player_range"] = string(playerRangeNode.getSourceRange(source));
    }

    ts::Node audienceNode = node.getChildByFieldName("has_audience");
    if (!audienceNode.isNull()) {
        configuration["audience"] = string(audienceNode.getSourceRange(source));
    }

    ts::Node setupNode = node.getChild(10); // set_rule
    auto size = setupNode.getNumChildren();
    ts::Node nestedNode = setupNode.getChildByFieldName("name");
    auto key = string(nestedNode.getSourceRange(source));

    string str1 = "";
    string str2 = "";
    ts::Cursor cursor = nestedNode.getNextSibling().getCursor();
    auto curr = cursor.getCurrentNode();
    for(size_t i = 0; i < size - 1; ++i){
        setupHelper(curr, source, str1, str2, key);
        if(!curr.isNull()){
            curr = curr.getNextSibling();
        }
    }

    // for debugging
    // std::cout << "Extracted Configuration Section:" << std::endl;
    // for (const auto& [key, value] : configuration) {
    //     std::cout << key << " : " << value << std::endl;
    // }
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
    ts::Node mapNode = node.getChildByFieldName("map");

    if (!mapNode.isNull()) {
        parseValueMap(mapNode, source, constants);
    }
    
    // for debugging
    // std::cout << "Extracted Constants Section:" << std::endl;
    // for (const auto& [key, entries] : constants) {
    //     std::cout << key << " :" << std::endl;
    //     for (const auto& [subKey, subValue] : entries) {
    //         std::cout << "  " << subKey << " : " << subValue << std::endl;
    //     }
    // }
}

void GameConfig::parseVariablesSection(const ts::Node& node, const string& source){
    parseMapHelper(node, source, variables);
}
    
void GameConfig::parseConfig(const string& fileContent){
    ts::Language language = tree_sitter_json();
    ts::Parser parser{language};
    ts::Tree tree = parser.parseString(fileContent);
    ts::Node root = tree.getRootNode();

    // for debugging
    // printTree(root, fileContent);

    for (size_t i = 0; i < root.getNumNamedChildren(); ++i) {
        ts::Node curr = root.getNamedChild(i);
        string sectionType = string(curr.getType());

        if (sectionType == "configuration") {
            parseConfigurationSection(curr, fileContent);
        } else if (sectionType == "constants") {
            parseConstantsSection(curr, fileContent);
        } else if (sectionType == "variables") {
            parseVariablesSection(curr, fileContent);
        } else if (sectionType == "per_player") {
            parsePerPlayerSection(curr, fileContent);
        } else if (sectionType == "per_audience") {
            parsePerAudienceSection(curr, fileContent);
        }
    }
}
