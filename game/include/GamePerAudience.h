#ifndef GAMEPERAUDIENCE_H
#define GAMEPERAUDIENCE_H

#include <unordered_map>
#include <string>

template <typename T>
class GamePerAudience {
    // for the Player.h
    std::unordered_map<std::string, T> perAudienceVariables;

    public:
        GamePerAudience() = default;
        ~GamePerAudience() = default;
        std::unordered_map<std::string, std::string> getAudienceVariables();
        T& addPerAudienceVariable(std::string key, T value);
        T& gamePerAudienceGetter(const string&) const;
        void gamePerAudienceSetter(const string&);

};

#endif 