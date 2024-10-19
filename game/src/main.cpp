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
    // std::cout << "Starting Rock-Paper-Scissors game!" << std::endl;

    // GameConfig config("Rock-Paper-Scissors");  
    // RockPaperScissorsGame game(config, 3);     

    // return 0;
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