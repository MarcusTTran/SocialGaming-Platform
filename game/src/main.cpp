#include "GameParser.h"
#include <iostream>

int main() {
    // this is my path to place txt file, correct it when you test it.
    string filename = "../../20241023_test/tree-sitter-test/CopyRPSConfig.txt";
    GameConfig parser(filename);
    
    cout << "Name: " << parser.getGameName() << endl;
    cout << "Range: " << parser.getPlayerRange().first << " ," << parser.getPlayerRange().second << endl;
    cout << "\nConfiguration Section:" << endl;
    parser.printMap();
    parser.printKeyValuePair();
    return 0;
}
