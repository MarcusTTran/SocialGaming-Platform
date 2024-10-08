// #include <iostream>
// #include <fstream>
// #include <sstream>
// #include <map>
// #include <string>
// #include <cpp-tree-sitter.h>
// #include "tree_sitter/api.h" // Include the Tree-sitter API header

// using namespace std;

// // Global maps for storing configuration sections
// map<string, string> configuration;

// // Declare the Tree-sitter function for social gaming language
// extern "C" { TSLanguage *tree_sitter_socialgaming(); }

// // Function to read the file content and return it as a string
// string readFileContent(const string& filePath) {
//     ifstream inputFile(filePath);
//     if (!inputFile) {
//         cerr << "Failed to open file: " << filePath << endl;
//         return "";
//     }
//     stringstream buffer;
//     buffer << inputFile.rdbuf();
//     return buffer.str();
// }

// // Function to extract the contents of the configuration section
// void parseConfigurationSection(const ts::Node& node, const string& source) {
//     // Extract each expected field based on the grammar definition
//     ts::Node nameNode = node.getChildByFieldName("name");
//     if (!nameNode.isNull()) {
//         configuration["name"] = string(nameNode.getSourceRange(source));
//     }

//     ts::Node playerRangeNode = node.getChildByFieldName("player_range");
//     if (!playerRangeNode.isNull()) {
//         configuration["player_range"] = string(playerRangeNode.getSourceRange(source));
//     }

//     ts::Node audienceNode = node.getChildByFieldName("has_audience");
//     if (!audienceNode.isNull()) {
//         configuration["audience"] = string(audienceNode.getSourceRange(source));
//     }

//     // Print the extracted fields for debugging
//     std::cout << "Extracted Configuration Section:" << std::endl;
//     for (const auto& [key, value] : configuration) {
//         std::cout << key << " : " << value << std::endl;
//     }
// }

// // Function to print the entire parse tree for debugging
// void printTree(const ts::Node& node, const string& source, int indent = 0) {
//     for (int i = 0; i < indent; ++i) std::cout << "  ";
//     std::cout << node.getType() << " -> " << node.getSourceRange(source) << std::endl;

//     for (size_t i = 0; i < node.getNumNamedChildren(); ++i) {
//         printTree(node.getNamedChild(i), source, indent + 1);
//     }
// }

// // Core function to parse the input file and identify configuration sections
// void parseConfigFile(const string& fileContent) {
//     // Initialize the parser with the SocialGaming language
//     ts::Language language = tree_sitter_socialgaming();
//     ts::Parser parser{language};
//     ts::Tree tree = parser.parseString(fileContent);
//     ts::Node root = tree.getRootNode();

//     std::cout << "Root Node Type: " << root.getType() << std::endl;

//     // Print the entire tree for analysis
//     std::cout << "Parse Tree Structure:" << std::endl;
//     printTree(root, fileContent);

//     // Traverse through the root node to parse each section
//     for (size_t i = 0; i < root.getNumNamedChildren(); ++i) {
//         ts::Node curr = root.getNamedChild(i);
//         string sectionType = string(curr.getType());

//         std::cout << "Found section: " << sectionType << std::endl;

//         // Check if the section is "configuration"
//         if (sectionType == "configuration") {
//             std::cout << "This node is a configuration section." << std::endl;
//             parseConfigurationSection(curr, fileContent);
//         }
//     }
//     std::cout << "Finish!" << std::endl;
// }

// int main() {
//     // Specify the path to the configuration file
//     string filename = "config.txt";

//     // Read the configuration file content using the readFileContent function
//     string configFileContent = readFileContent(filename);

//     // Parse the configuration file content
//     parseConfigFile(configFileContent);

//     std::cout << "Configuration Section:" << std::endl;
//     for (const auto& [key, value] : configuration) {
//         std::cout << key << " : " << value << std::endl;
//     }

//     return 0;
// }


#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <cpp-tree-sitter.h>
#include "tree_sitter/api.h" 

using namespace std;


map<string, string> configuration;
map<string, string> variables;
map<string, string> perPlayer;
map<string, string> perAudience;
map<string, map<string, string>> constants;  


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

string extractStringValue(const ts::Node& node, const string& source) {
    if (node.getType() == "quoted_string") {
        ts::Node textNode = node.getNamedChild(0);  
        if (!textNode.isNull() && textNode.getType() == "string_text") {
            return string(textNode.getSourceRange(source));  
        }
    }
    return string(node.getSourceRange(source));  
}

void parseValueMapEntry(const ts::Node& valueMapNode, const string& source, map<string, string>& outputMap) {
    string name, beats;
    for (size_t i = 0; i < valueMapNode.getNumNamedChildren(); ++i) {
        ts::Node mapEntry = valueMapNode.getNamedChild(i);
        ts::Node mapKey = mapEntry.getChildByFieldName("key");
        ts::Node mapValue = mapEntry.getChildByFieldName("value");

        if (!mapKey.isNull() && !mapValue.isNull()) {
            string mapKeyStr = string(mapKey.getSourceRange(source));
            string mapValueStr = extractStringValue(mapValue, source); 

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

void parseValueMap(const ts::Node& node, const string& source, map<string, map<string, string>>& outputMap) {
    for (size_t i = 0; i < node.getNumNamedChildren(); ++i) {
        ts::Node entryNode = node.getNamedChild(i);
        if (entryNode.getType() == "map_entry") {
            ts::Node keyNode = entryNode.getChildByFieldName("key");
            ts::Node valueNode = entryNode.getChildByFieldName("value");

            if (!keyNode.isNull() && !valueNode.isNull()) {
                string key = string(keyNode.getSourceRange(source));

                if (valueNode.getType() == "list_literal") {
                    parseListLiteral(valueNode, source, outputMap[key]);
                } else {
                    outputMap[key][""] = extractStringValue(valueNode, source);
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

    // ignore setup section atm

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

    std::cout << "Extracted Constants Section:" << std::endl;
    for (const auto& [key, entries] : constants) {
        std::cout << key << " :" << std::endl;
        for (const auto& [subKey, subValue] : entries) {
            std::cout << "  " << subKey << " : " << subValue << std::endl;
        }
    }
}

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

    std::cout << "\nConstants Section:" << std::endl;
    for (const auto& [key, entries] : constants) {
        std::cout << key << " :" << std::endl;
        for (const auto& [subKey, subValue] : entries) {
            std::cout << "  " << subKey << " : " << subValue << std::endl;
        }
    }

    return 0;
}






