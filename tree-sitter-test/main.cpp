#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <variant>
#include <cpp-tree-sitter.h>
#include "tree_sitter/api.h" 

using namespace std;

using configVariable = std::variant<map<string, string>>;

vector<string> toSkip = {"[", "]", ",", "{", "}", ":", "\"", "(", ")"};
map<string, string> configuration;
map<string, vector<pair<pair<string, string>, pair<string, string>>>> variables;
map<string, vector<pair<pair<string, string>, pair<string, string>>>> perPlayer;
map<string, vector<pair<pair<string, string>, pair<string, string>>>> perAudience;
map<string, vector<pair<pair<string, string>, pair<string, string>>>> constants;  
map<string, vector<map<string, string>>> setup;

extern "C" { TSLanguage *tree_sitter_socialgaming(); }

// Function to read the file content and return it as a string
string readFileContent(const string& filePath) {
    ifstream inputFile(filePath);
    if (!inputFile) {
        cerr << "Failed to open file: " << filePath << endl;
        return "";
    }
    stringstream buffer;
    buffer << inputFile.rdbuf();
    return buffer.str();
}

void extractStringValue(const ts::Node& node, const std::string& source, pair<string, string> &str1, pair<string, string> &str2, std::string keyID, map<string, vector<pair<pair<string, string>, pair<string, string>>>>& output){
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
        extractStringValue(child, source, str1, str2, keyID, output);
    }
    if(!str1.first.empty() && !str1.second.empty() && !str2.first.empty() && !str2.second.empty()){
        output[keyID].push_back({str1, str2});
        str1 = {};
        str2 = {};
    }
}

template <typename T>
void parseValueMap(const ts::Node& node, const string& source, T& outputMap) {
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
                extractStringValue(valueNode, source, str1, str2, key, outputMap);   
            }
        }
    }
}

/*
  setup: {
    rounds {
      kind: integer
      prompt: "The number of rounds to play"
      range: (1, 20)
    }
  }
*/
void setupHelper(const ts::Node& node, const string& source, string& str1, string& str2, const string& keyID) {
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

void parseConfigurationSection(const ts::Node& node, const string& source) {
    ts::Node nameNode = node.getChildByFieldName("name");
    if (!nameNode.isNull()) {
        configuration["name"] = string(nameNode.getChild(1).getSourceRange(source));
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

void parseConstantsSection(const ts::Node& node, const string& source) {
    parseValueMap(node.getChildByFieldName("map"), source, constants);
    
    // for debugging
    // std::cout << "Extracted Constants Section:" << std::endl;
    // for (const auto& [key, entries] : constants) {
    //     std::cout << key << " :" << std::endl;
    //     for (const auto& [subKey, subValue] : entries) {
    //         std::cout << "  " << subKey << " : " << subValue << std::endl;
    //     }
    // }
}

void parseVariablesSection(const ts::Node& node, const string& source) {
    parseValueMap(node.getChildByFieldName("map"), source, variables);
}

void parsePerPlayerSection(const ts::Node& node, const string& source) {
    parseValueMap(node.getChildByFieldName("map"), source, perPlayer);
}

void parsePerAudienceSection(const ts::Node& node, const string& source) {
    parseValueMap(node.getChildByFieldName("map"), source, perAudience);
}

// to print better look how tree-sitter looks like behind the scene
void printTree(const ts::Node& node, const string& source, int indent = 0) {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << node.getType() << " -> " << node.getSourceRange(source) << std::endl;

    for (size_t i = 0; i < node.getNumNamedChildren(); ++i) {
        printTree(node.getNamedChild(i), source, indent + 1);
    }
}

void parseConfigFile(const string& fileContent) {
    ts::Language language = tree_sitter_socialgaming();
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


int main() {
    string filename = "config.txt";
    string configFileContent = readFileContent(filename);
    parseConfigFile(configFileContent);

    std::cout << "\nConfiguration Section:" << std::endl;
    for (const auto& [key, value] : configuration) {
        std::cout << key << " : " << value << std::endl;
    }

    std::cout << "\nSetup Section:" << std::endl;
    for(const auto& [key, value] : setup){
        cout << key << ":" << endl;
        for(const auto& ele : value){
            for(const auto& [k, v] : ele){
                cout << k << ": " << v << endl;
            }
        }
    }
    
    std::cout << "\nConstants Section:" << std::endl;
    for (const auto& [key, entries] : constants) {
        std::cout << key << " :" << std::endl;
        for(const auto& ele : entries){
            auto pair1 = ele.first;
            auto pair2 = ele.second;
            std::cout << pair1.first << ": " << pair1.second << " ";
            std::cout << pair2.first << ": " << pair2.second;
            cout << std::endl;
        }   
    }

    std::cout << "\nVariable Section:" << std::endl;
    for (const auto& [key, entries] : variables) {
        std::cout << key << " :" << std::endl;
        for(const auto& ele : entries){
            auto pair1 = ele.first;
            auto pair2 = ele.second;
            std::cout << pair1.first << ": " << pair1.second << " ";
            std::cout << pair2.first << ": " << pair2.second;
            cout << std::endl;
        }   
    }

    std::cout << "\nPerPlayer Section:" << std::endl;
    for (const auto&[key, entries] : perPlayer) {
        std::cout << key << " :" << std::endl;
        for(const auto& ele : entries){
            auto pair1 = ele.first;
            auto pair2 = ele.second;
            std::cout << pair1.first << ": " << pair1.second << " ";
            std::cout << pair2.first << ": " << pair2.second;
            cout << std::endl;
        }   
    }

    std::cout << "\nPerAudience Section:" << std::endl;
    for (const auto& [key, entries] : perAudience) {
        std::cout << key << " :" << std::endl;
        for(const auto& ele : entries){
            auto pair1 = ele.first;
            auto pair2 = ele.second;
            std::cout << pair1.first << ": " << pair1.second << " ";
            std::cout << pair2.first << ": " << pair2.second;
            cout << std::endl;
        }   
    }
    
    return 0;
}
