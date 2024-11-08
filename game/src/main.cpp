#include "ParserRulesAPI.h"
#include <string>

int main(){
    std::string filename = "../../config/config.game";
    ParserRulesAPI parserAPI(filename);
    parserAPI.initialize();
    parserAPI.printToConsole();
    parserAPI.runRules();
    return 0;
}