#include "ParserRulesAPI.h"
#include <string>

int main(){
    std::string filename = "config.txt";
    ParserRulesAPI middleMan(filename);
    middleMan.initialize();
    middleMan.printToConsole();
    middleMan.runRules();
    return 0;
}