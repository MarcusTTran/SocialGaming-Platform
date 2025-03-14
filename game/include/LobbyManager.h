#pragma once

#include "Lobby.h"
#include "Messenger.h"
#include "game.h"


/*
 *  The LobbyManager is responsible for managing the creation and deletion of
 *  Lobbies, and for routing messages to the correct Lobby.
 */
class LobbyManager {

public:
    LobbyManager(std::shared_ptr<IServer> server) : server(server) {}
    ~LobbyManager() = default;
    void createLobby(std::unique_ptr<Game> game, const networking::Connection &lobbyCreator);
    void addPlayerToLobby(const std::string &lobbyCode, const networking::Connection &connection);
    void routeMessage(const networking::Connection &connection, const std::string &message);

    // These methods are used for managing pending display names
    bool isAwaitingDisplayName(const networking::Connection &connection) const;
    void addPlayerToLobbyWithDisplayName(const networking::Connection &connection, const std::string &displayName);

    bool isInLobby(const networking::Connection &connection) const;
    bool isLobbyCreator(const networking::Connection &connection) const;
    bool isDisplayNameUnique(const std::string &lobbyCode, const std::string &displayName) const;
    bool isLobbyCodeValid(const std::string &lobbyCode) const;
    bool isWaitingForLobbyCode(const networking::Connection &connection) const;
    void addConnectionWaitingForLobbyCode(const networking::Connection &connection);
    void updateLobbies();
    std::string generateWelcomeMessage();
    std::map<std::string, std::unique_ptr<Lobby>> &getLobbies() { return lobbies; }

private:
    Lobby *findLobbyByCode(const std::string &lobbyCode) const;
    void removeFromPendingDisplayNames(const networking::Connection &connection);
    std::string generateLobbyCode();

    // Maps for tracking pending display names
    std::unordered_map<uintptr_t, std::string> pendingDisplayNames;

    // Map for tracking all created lobbies currently active
    std::map<std::string, std::unique_ptr<Lobby>> lobbies;

    std::shared_ptr<IServer> server;

    // Maps for tracking players in lobbies and lobby creators
    // Key is the player's connection id and value is the lobby code
    // Used for routing messages to the correct lobby
    std::map<uintptr_t, std::string> playersInLobbies;
    std::map<uintptr_t, std::string> lobbyCreators;
    std::vector<networking::Connection> waitingForLobbyCode;
};