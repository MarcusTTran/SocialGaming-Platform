// author: kwa132, Mtt8

#include <string>
#include <vector>
#include <map>
#include <cpp-tree-sitter.h>

using namespace std;

class GameConfig {
public:
    GameConfig(const string& configFileContent);

    string getGameName() const;
    pair<int, int> getPlayerRange() const;
    bool hasAudience() const;
    map<string, string> getConstants();
    map<string, string> getVariables();
    void gameNameSetter(const string&);
    void rangeSetter(const pair<int, int>&);
    void audienceSetter(bool);
    void constantsSetter(const string&, const string&);
    void variablesSetter(const string&, const string&);
    void displayGameConfig();
    pair<string, string> parserHelper(const ts::Node&, size_t, const string&);
    map<string, string> getRules() const;
private:
    string gameName;
    pair<int, int> playerRange;
    bool audience;
    map<string, string> constants;
    map<string, string> variables;
    map<string, string> weaponsRules; 

    void parseConfig(const string& configFileContent);
    void parseConfigurationSection(const ts::Node& configNode, const string& source);
    void parseConstantsSection(const ts::Node& constantsNode, const string& source);
    void parseVariablesSection(const ts::Node& variablesNode, const string& source);
    void parseWeapons(const std::string&);
};
