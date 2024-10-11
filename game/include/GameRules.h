#ifndef GAMERULES_H
#define GAMERULES_H

#include <unordered_map>
#include <string>

// - Contains the structure of the game using a social gaming file language as pseudocode
// - High-level control statements include: for, while, parallel for, in parallel, match
// - Other statements may start with an operation like "discard" or "message all"
// - Often, the rules will refer to other variabels or constants define in other parts of the game file "ie. configuration.rounds.upFrom(1)"
class GameRules {
    

    public:
        
        GameRules();
        ~GameRules();
};

#endif // GAMERULES_H
