#pragma once
#include "GameParser.h"
#include "RulesRunner.h"
#include <iostream>
#include <string>


/*
    This class is for testing GameParser and RulesRunner purpose

    This is an API between GameParser and RulesRunner

    GameParser: responsible for parsing data from input txt file
    RulesRunner: responsible for running game's rules
    ParserRulesAPI: responsible for communication in between
*/

class ParserRulesAPI {
public:
    ParserRulesAPI(const std::string &configFilePath);
    void initialize();
    void runRules();
    void printToConsole();

private:
    std::unique_ptr<ParsedGameData> parser;
    std::unique_ptr<RulesRunner> rulesRunner;
};