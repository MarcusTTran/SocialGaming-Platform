#include "GameParser.h"
#include <iostream>

int main() {
  //////////////////////////////////////////////////////////////////
  // This is for testing parsing result. Correct the file path and
  // the result will be printed to console.
  //////////////////////////////////////////////////////////////////
  std::string filename = "../config/rockPaperScissors.game";
  GameConfig parser(filename);

  std::cout << "Name: " << parser.getGameName() << std::endl;
  std::cout << "Range: " << parser.getPlayerRange().first << " ,"
            << parser.getPlayerRange().second << std::endl;
  std::cout << "\nConfiguration Section:" << std::endl;
  parser.printMap();
  parser.printKeyValuePair();
  return 0;
}
