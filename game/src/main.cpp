#include "rockPaperScissors.h"
#include <iostream>

int main() {
    std::cout << "Starting Rock-Paper-Scissors game!" << std::endl;

    GameConfig config("Rock-Paper-Scissors");  
    RockPaperScissorsGame game(config, 3);     

    return 0;
}
