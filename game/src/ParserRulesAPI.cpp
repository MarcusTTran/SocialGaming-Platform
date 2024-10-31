#include "ParserRulesAPI.h"

ParserRulesAPI::ParserRulesAPI(const std::string& configFilePath) : parser(std::make_unique<GameConfig>(configFilePath)),
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
    for(const auto& setup : configuration.setup){
        for(const auto& [key, value] : setup){
            std::cout << key << ":" << std::endl;
            for(const auto& ele : value){
                for(const auto& [k, v] : ele){
                    std::cout << k << ": " << v << std::endl;
                }
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
            std::cout << std::endl;
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
            std::cout << std::endl;
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
            std::cout << std::endl;
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
            std::cout << std::endl;
        }   
    }
    
    // std::cout << "\nRule Section:" << std::endl;
    // for(const auto& rule : rules){
    //     printRule(rule);
    //     implementForRule(rule);
    // }
}

void ParserRulesAPI::runRules(){
    rulesRunner->processRules();
}