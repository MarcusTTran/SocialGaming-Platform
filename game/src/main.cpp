#include "GameParser.h"
#include <iostream>

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

int main() {
    string filename = "../config.txt";
    string configFileContent = readFileContent(filename);
    GameConfig parser(configFileContent);

    std::cout << "\nConfiguration Section:" << std::endl;
    for (const auto& [key, value] : parser.getConfiguration()) {
        std::cout << key << " : " << value << std::endl;
    }

    std::cout << "\nSetup Section:" << std::endl;
    for(const auto& [key, value] : parser.getSetup()){
        cout << key << ":" << endl;
        for(const auto& ele : value){
            for(const auto& [k, v] : ele){
                cout << k << ": " << v << endl;
            }
        }
    }
    
    std::cout << "\nConstants Section:" << std::endl;
    for (const auto& [key, entries] : parser.getConstants()) {
        std::cout << key << " :" << std::endl;
        for(const auto& ele : entries){
            auto pair1 = ele.first;
            auto pair2 = ele.second;
            std::cout << pair1.first << ": " << pair1.second << " ";
            std::cout << pair2.first << ": " << pair2.second;
            cout << std::endl;
        }   
    }

    std::cout << "\nVariables Section:" << std::endl;
    for (const auto& [key, value] : parser.getVariables()) {
        std::cout << key << " : " << value << std::endl;
    }

    std::cout << "\nPer-Player Section:" << std::endl;
    for (const auto& [key, value] : parser.getPerPlayer()) {
        std::cout << key << " : " << value << std::endl;
    }

    std::cout << "\nPer-Audience Section:" << std::endl;
    for (const auto& [key, value] : parser.getPerAudience()) {
        std::cout << key << " : " << value << std::endl;
    }
    
    return 0;
}
