#ifndef GAMEVARIABLES_H
#define GAMEVARIABLES_H

#include <unordered_map>
#include <string>

template <typename T>
class GameVariables {

    std::unordered_map<std::string, T> variables;

    public:
        GameVariables() = default;
        ~GameVariables() = default;
        std::unordered_map<std::string, T>& getVariables() {
            return variables;
        }
        T& gameVariableGetter(const string&) const;
        void gameVariableSetter(const string&);
       
};


#endif // GAMEVARIABLES_H