#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <cpp-tree-sitter.h>
#include "tree_sitter/api.h" 

using namespace std;

vector<string> toSkip = {"[", "]", ",", "{", "}", ":"};
map<string, string> configuration;
map<string, string> variables;
map<string, string> perPlayer;
map<string, string> perAudience;
map<string, vector<map<string, string>>> constants;  
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

// fst function to try to parse sting
string extractStringValue(const ts::Node& node, const string& source) {
    if (node.getType() == "quoted_string") {
        ts::Node textNode = node.getNamedChild(0);  
        if (!textNode.isNull() && textNode.getType() == "string_text") {
            std::cout << string(textNode.getSourceRange(source)) << std::endl;
            return string(textNode.getSourceRange(source));  
        }
    }
    for(auto child : ts::Children{node}){
        auto type = string(child.getType());
        if(find(begin(toSkip), end(toSkip), type) != end(toSkip)){
            continue;
        }
        std::cout << child.getType() << std::endl; 
        extractStringValue(child, source);
    }
    return string(node.getSourceRange(source));  
}

// needs to fix this as being more general, atm, it is foucsing on RPS constans parse
void parseValueMapEntry(const ts::Node& valueMapNode, const string& source, map<string, string>& outputMap) {
    string name, beats;

    for (size_t i = 0; i < valueMapNode.getNumNamedChildren(); ++i) {
        ts::Node mapEntry = valueMapNode.getNamedChild(i);
        ts::Node mapKey = mapEntry.getChildByFieldName("key");
        ts::Node mapValue = mapEntry.getChildByFieldName("value");

        if (!mapKey.isNull() && !mapValue.isNull()) {
            string mapKeyStr = string(mapKey.getSourceRange(source));
            string mapValueStr = extractStringValue(mapValue, source); 
            std::cout << mapValueStr << std::endl;
            if (mapKeyStr == "name") {
                name = mapValueStr;  
            } else if (mapKeyStr == "beats") {
                beats = mapValueStr;  
            }
        }
    }
    if (!name.empty() && !beats.empty()) {
        outputMap[name] = beats;  
    }
}

void parseListLiteral(const ts::Node& listNode, const string& source, map<string, string>& outputMap) {
    for (size_t i = 0; i < listNode.getNumNamedChildren(); ++i) {
        ts::Node element = listNode.getNamedChild(i);

        if (element.getType() == "value_map") {
            parseValueMapEntry(element, source, outputMap);
        }
    }
}

// snd function to try to parse sting recursively
void extractStringValue2(const ts::Node& node, const std::string& source, std::string &str1, std::string &str2, std::string keyID){
    if (node.getType() == "quoted_string") {
        ts::Node textNode = node.getNamedChild(0);  
        if (!textNode.isNull() && textNode.getType() == "string_text") {
            std::cout << string(textNode.getSourceRange(source)) << std::endl;
            if(str1.empty()){
                str1 = string(textNode.getSourceRange(source));
            } else if(str2.empty()){
                str2 = string(textNode.getSourceRange(source));
                constants[keyID].push_back({ {str1, str2} });
                str1.clear();
                str2.clear();
            }
            return;
        }
    }
    for(auto child : ts::Children{node}){
            auto type = string(child.getType());
        if(find(begin(toSkip), end(toSkip), type) != end(toSkip)){
            continue;
        }
        std::cout << child.getType() << std::endl; 
        extractStringValue2(child, source, str1, str2, keyID);
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

void parseSetup(const ts::Node& node, const string& source, size_t idx, const string& subSectionName){
        std::cout << "////////////2///////////" << node.getNumNamedChildren() << std::endl;
        std::cout << node.getSourceRange(source) << endl;

    for(size_t i = 0; i < node.getNumNamedChildren(); ++i){
        std::cout << "////////////3///////////" << std::endl;

        ts::Node kindNode = node.getChildByFieldName("kind");
        if (!kindNode.isNull()) {
            std::cout << "////////////4///////////" << std::endl;
            std::cout << string(kindNode.getSourceRange(source)) << std::endl;

            setup[subSectionName][idx]["kind"] = string(kindNode.getSourceRange(source));
        }

        ts::Node promptNode = node.getChildByFieldName("prompt");
        if (!promptNode.isNull()) {
            std::cout << "////////////5///////////" << std::endl;
            std::cout << string(promptNode.getSourceRange(source)) << std::endl;
            setup[subSectionName][idx]["prompt"] = string(promptNode.getSourceRange(source));
        }

        // (2, 4)
        ts::Node rangeNode = node.getChildByFieldName("range");
        if (!rangeNode.isNull()){
            std::cout << string(rangeNode.getSourceRange(source)) << std::endl;
            // later to deal with type convertin
            // auto rangeNodeStr = rangeNode.getSourceRange(source);
            // auto firstNum = rangeNodeStr.substr(1, 1);
            // auto secondNum = rangeNodeStr.substr(4, 1);
            ts::Node firstNum = rangeNode.getChild(1);
            ts::Node secondNum = rangeNode.getChild(3);
            string_view firstNumVal = firstNum.getSourceRange(source);
            string_view secondNumVal = secondNum.getSourceRange(source);
            string res = string(firstNumVal) + ", " + string(secondNumVal);
            setup[subSectionName][idx]["range"] = res;
        }
    }
}

void parseValueMap(const ts::Node& node, const string& source, map<string, vector<map<string, string>>>& outputMap) {
    for (size_t i = 0; i < node.getNumNamedChildren(); ++i) {
        ts::Node entryNode = node.getNamedChild(i);
        if (entryNode.getType() == "map_entry") {
            ts::Node keyNode = entryNode.getChildByFieldName("key");
            ts::Node valueNode = entryNode.getChildByFieldName("value");
            if (!keyNode.isNull() && !valueNode.isNull()) {
                string key = string(keyNode.getSourceRange(source));

                if (valueNode.getType() == "list_literal") {
                    // need to figure out a better way to parse
                    // parseListLiteral(valueNode, source, outputMap[key]);
                } else {
                    string str1 = "";
                    string str2 = "";
                    extractStringValue2(valueNode, source, str1, str2, key);
                }
            }
        }
    }
}

void parseConfigurationSection(const ts::Node& node, const string& source) {
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

    //////////////////////////////////////////////////////////////////////////////////
    // ignore setup section atm
    // std::cout << "Testing" << std::endl;
    // ts::Node setupNode = node.getChild(10); // set_rule
    // ts::Node nestedNode = setupNode.getChildByFieldName("name");
    // std::cout << nestedNode.getType() << std::endl;
    // ts::Node temp = setupNode.getChild(2);
    // if(!setupNode.isNull()){
    //     auto subSectionName = string(nestedNode.getSourceRange(source)); // rounds
    //     nestedNode = nestedNode.getChild(1);
    //     cout << temp.getType() << endl;
    //     cout << temp.getSourceRange(source) << endl;
    //     for(size_t i = 0; i < nestedNode.getNumChildren(); ++i){
    //         auto curr = nestedNode.getNamedChild(i);
    //         parseSetup(curr, source, i, subSectionName);
    //     }
    // }
    //////////////////////////////////////////////////////////////////////////////////


    std::cout << "Extracted Configuration Section:" << std::endl;
    for (const auto& [key, value] : configuration) {
        std::cout << key << " : " << value << std::endl;
    }
}

void parseConstantsSection(const ts::Node& node, const string& source) {
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

    std::cout << "Root Node Type: " << root.getType() << std::endl;
    std::cout << "Parse Tree Structure:" << std::endl;
    printTree(root, fileContent);

    for (size_t i = 0; i < root.getNumNamedChildren(); ++i) {
        ts::Node curr = root.getNamedChild(i);
        string sectionType = string(curr.getType());

        if (sectionType == "configuration") {
            parseConfigurationSection(curr, fileContent);
        } else if (sectionType == "constants") {
            parseConstantsSection(curr, fileContent);
        } 
    }
    std::cout << "Finish!" << std::endl;
}


int main() {
    string filename = "config.txt";
    string configFileContent = readFileContent(filename);
    parseConfigFile(configFileContent);

    std::cout << "\nConfiguration Section:" << std::endl;
    for (const auto& [key, value] : configuration) {
        std::cout << key << " : " << value << std::endl;
    }

    // std::cout << "\nSetup Section:" << std::endl;
    // for(const auto& ele : setup){
    //     for(const auto& [key, value] : ele){
    //         std::cout << key << " :" << value << endl; 
    //     }
    // }
    
    std::cout << "\nConstants Section:" << std::endl;
    for (const auto& [key, entries] : constants) {
        std::cout << key << " :" << std::endl;
        for(const auto& ele : entries){
            for(const auto& [k, v] : ele){
                std::cout << k << " :" << v << std::endl;
            }
        }
    }
    return 0;
}






