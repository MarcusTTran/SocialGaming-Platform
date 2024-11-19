#include "ParserRulesAPI.h"

ParserRulesAPI::ParserRulesAPI(const std::string& configFilePath) : parser(std::make_unique<ParsedGameData>(configFilePath)),
rulesRunner(std::make_unique<RulesRunner>()){}

void ParserRulesAPI::initialize(){
    // parser->parseConfig();

    rulesRunner->configurationSetter(parser->getConfiguration());
    rulesRunner->constantsSetter(parser->getConstants());
    rulesRunner->variablesSetter(parser->getVariables());
    rulesRunner->perPlayerSetter(parser->getPerPlayer());
    rulesRunner->perAudienceSetter(parser->getPerAudience());
    rulesRunner->rulesSetter(parser->getRules());
}

void ParserRulesAPI::printToConsole(){

    auto configuration = parser->getConfiguration();
    auto constants = parser->getConstants();
    auto variables = parser->getVariables();
    auto perPlayer = parser->getPerPlayer();
    auto perAudience = parser->getPerAudience();

    std::cout << "\nConfiguration Section:" << std::endl;
    // for (const auto& [key, value] : configuration) {
    //     std::cout << key << " : " << value << std::endl;
    // }
    std::cout << "name: " << configuration.name << std::endl;
    std::cout << "player range: (" << configuration.range.first << ", " << configuration.range.second << ")" << std::endl;
    std::cout << "audience: " << (configuration.audience == true ? "true" : "false") << std::endl;
    
    std::cout << "\nSetup Section:" << std::endl;
    for (const auto& setupEntry : configuration.setup) {
        for (const auto& [key, value] : setupEntry) {
            std::cout << key << ":\n";
            // Print the DataValue, assuming we have a method in DataValue to handle printing
            value.print(std::cout, 2); // Using the DataValue print method to display values with indentation
        }
    }

    std::cout << "\nConstants Section:" << std::endl;
    parser->printDataValue(constants);

    std::cout << "\nVariables Section:" << std::endl;
    parser->printDataValue(variables);

    std::cout << "\nPerPlayer Section:" << std::endl;
    parser->printDataValue(perPlayer);

    std::cout << "\nPerAudience Section:" << std::endl;
    parser->printDataValue(perAudience);
}

void ParserRulesAPI::runRules(){
    rulesRunner->processRules();
}