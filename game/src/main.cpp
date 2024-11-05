#include "ParserRulesAPI.h"
#include "GameConfiguration.h"
#include "GameParser.h"
#include <iostream>
#include <string>

int main(){
    std::string filename = "config.game";
    ParserRulesAPI parserAPI(filename);
    parserAPI.initialize();
    parserAPI.printToConsole();
    parserAPI.runRules();

    /*
        This is the demo section to show how to extract data from GameConfiguration class, interacting
        with GameParser.
    */

    ParsedGameData parser(filename);
    GameConfiguration config(parser);
    std::cout << "Name: " << config.getGameName().getName() << std::endl;
    std::cout << "Player Range: " << config.getPlayerRange().first << ", " << 
        config.getPlayerRange().second << std::endl;
    std::cout << "Audience: " << config.hasAudience() << std::endl;
    auto setups = config.getSetup();
    for (const auto& setup : setups) {
        std::cout << "Setup Name: " << setup.name << std::endl;
        if (setup.kind.has_value()) {
            std::cout << "Setup Kind: " << *setup.kind << std::endl;
        } 
        if (setup.prompt.has_value()) {
            std::cout << "Setup prompt: " << *setup.prompt << std::endl;
        } 

        if (setup.getRange().has_value()) {
            auto range = setup.getRange().value();
            std::cout << "Range: (" << range.first << ", " << range.second << ")" << std::endl;
        } 

        if (setup.getChoices().has_value()) {
            auto choices = setup.getChoices().value();
            std::cout << "Choices:" << std::endl;
            for (const auto& [key, description] : choices) {
                std::cout << "  " << key << ": " << description << std::endl;
            }
        } 

        if (setup.getDefault().has_value()) {
            auto defaultValue = setup.getDefault().value();
            std::cout << "Default Value:" << std::endl;
            for (const auto& [key, value] : defaultValue) {
                std::cout << "  " << key << ": " << value.getType() << std::endl;
            }
        } 
    }
    return 0;
}