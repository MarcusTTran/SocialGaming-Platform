#include "GameParser.h"
#include <iostream>

int main() {
    // this is my path to place txt file, correct it when you test it.
    std::string filename = "../../20241023_test/tree-sitter-test/CopyRPSConfig.txt";
    GameConfig parser(filename);
    
    std::cout << "Name: " << parser.getGameName() << std::endl;
    std::cout << "Range: " << parser.getPlayerRange().first << " ," << parser.getPlayerRange().second << std::endl;
    std::cout << "\nConfiguration Section:" << std::endl;
    parser.printMap();
    parser.printKeyValuePair();
    return 0;
}
