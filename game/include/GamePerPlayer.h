#ifndef GAMEPERPLAYER_H
#define GAMEPERPLAYER_H


template <typename T>
class GamePerPlayer {
    std::unordered_map<std::string, T> perPlayerVariables;
    
    public:
        GamePerPlayer() = default;
        ~GamePerPlayer() = default;
        std::unordered_map<std::string, std::string> getPlayerVariables();
        T& addPerPlayerVariable(std::string key, T value);
        T& gamePerPlayerGetter(const string&) const;     
        void gamePerPlayerSetter(const string&);  // perPlayerVariables[key] = winner = 3; 
    // For PLayer.h
};

#endif // GAMEPERPLAYER_H