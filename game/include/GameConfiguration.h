// author: kwa132, mtt8

#include <string>
#include <vector>
#include <optional>
#include <utility>
#pragma once


class GameName {
    private:    
        const std::string& name;
    public:
        // We will not allow an instance to be create without an input string
        GameName(const std::string& name) : name(name) {} 
       
};

class SetupRules {
    public:
        class SetupRuleKind {
            // Configuration kinds can be 'boolean', 'integer', 'string', 'enum', 'question-answer', 'multiple-choice', 'json'.
            std::string name;
            SetupRuleKind(const std::string& name) : name(name) {}
            // TODO: create a validation function that checks if the Kind is valid (ie. 'integer')
            // TODO: perhaps make this a template? Would that work?
        };

        SetupRuleKind kind;
        std::string prompt;
        std::optional< std::pair<size_t, size_t> > range;
        std::optional< std::vector<string> >  choices;
        // TODO: Perhaps add a default? It is used for range in professors example of RPS
};

class GameConfiguration {
    private:
        const GameName gameName;
        std::pair<size_t, size_t> playerRange;
        bool audience; 
        // TODO: change
        std::vector<SetupRules> setup; // Contains a list of SetupRules (like the number of rounds to start from)
        
    public:
        ~GameConfiguration() = default;  
        
        std::string getGameName() const; 
        std::pair<size_t, size_t> getPlayerRange() const;
        bool hasAudience() const;
        std::vector<SetupRules> getSetup();
};

