#include "LobbyManager.h"
#include <algorithm>
#include <random>

class Game;

void LobbyManager::createLobby(std::unique_ptr<Game> game, const networking::Connection &lobbyCreator) {

    // Check if the connection has already created a lobby
    // Dont allow a player to create multiple lobbies
    // Check if the connection is already in a lobby
    // Dont allow a player to create a lobby if they are already in a lobby
    if (isLobbyCreator(lobbyCreator) || isInLobby(lobbyCreator)) {
        return;
    }

    std::cout << "Creating lobby..." << std::endl;

    std::string lobbyCode = generateLobbyCode();
    lobbies.emplace(lobbyCode,
                    std::make_unique<Lobby>(std::move(game), server,
                                            std::make_shared<networking::Connection>(lobbyCreator), lobbyCode));

    // Track that this connection is the creator of the lobby
    lobbyCreators[lobbyCreator.id] = lobbyCode;

    // Debug statement to verify the lobby creation
    std::cout << "Lobby created with code: " << lobbyCode << std::endl;

    // Print out all lobby codes.
    std::cout << "Lobby codes: ";
    for (const auto &lobby : lobbies) {
        std::cout << lobby.first << " ";
    }
    std::cout << std::endl;
}

std::string LobbyManager ::generateLobbyCode() {
    static constexpr std::string_view digits = "0123456789";
    static const int codeLength = 6;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::string newCode;
    newCode.reserve(codeLength);

    do {
        newCode.clear();
        std::generate_n(std::back_inserter(newCode), codeLength,
                        [&]() { return digits[std::uniform_int_distribution<>(0, digits.size() - 1)(gen)]; });
    } while (lobbies.find(newCode) != lobbies.end()); // Check for uniqueness

    return newCode;
}

// This method is called when a player wants to join a lobby using a lobby code
// The player is prompted to enter a display name if the lobby is found
void LobbyManager::addPlayerToLobby(const std::string &lobbyCode, const networking::Connection &connection) {

    // Check if the connection is already in a lobby
    // Dont allow a player to join multiple lobbies
    // Check if the connection is a lobby creator
    // Dont allow a player to join a lobby if they are the creator of a lobby
    if (isInLobby(connection) || isLobbyCreator(connection)) {
        return;
    }

    auto lobby = findLobbyByCode(lobbyCode);
    if (lobby) {

        if (lobby->getState() != Lobby::LobbyState::Waiting) {
            server->sendToConnection("The game has already started. Please try again later.", connection);
            return;
        }
        // Track that this connection is awaiting a display name
        pendingDisplayNames[connection.id] = lobbyCode;
        server->sendToConnection("Lobby found. Please enter a display name:", connection);

        // Remove the connection from the waiting list
        auto found = std::find(waitingForLobbyCode.begin(), waitingForLobbyCode.end(), connection);
        if (found != waitingForLobbyCode.end()) {
            waitingForLobbyCode.erase(found);
        }

    } else {
        server->sendToConnection("Lobby not found. Please check the code and try again.", connection);
    }
}

// This method is called when a player has entered a display name and is ready to join the lobby
void LobbyManager::addPlayerToLobbyWithDisplayName(const networking::Connection &connection,
                                                   const std::string &displayName) {
    std::string lobbyCode = pendingDisplayNames[connection.id];
    auto lobby = findLobbyByCode(lobbyCode);
    if (lobby && isDisplayNameUnique(lobbyCode, displayName)) {
        Player player{connection, displayName};
        lobby->addPlayer(player);
        playersInLobbies[connection.id] = lobbyCode;
        removeFromPendingDisplayNames(connection);
    } else {
        server->sendToConnection("Display name is already taken. Please enter a different display name:", connection);
    }
}

Lobby *LobbyManager::findLobbyByCode(const std::string &lobbyCode) const {
    auto it = lobbies.find(lobbyCode);
    if (it != lobbies.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool LobbyManager::isAwaitingDisplayName(const networking::Connection &connection) const {
    return pendingDisplayNames.find(connection.id) != pendingDisplayNames.end();
}

void LobbyManager::removeFromPendingDisplayNames(const networking::Connection &connection) {
    pendingDisplayNames.erase(connection.id);
}

void LobbyManager::routeMessage(const networking::Connection &connection, const std::string &message) {

    // Check if the connection is a lobby creator
    auto lobbyCreator = lobbyCreators.find(connection.id);

    if (lobbyCreator != lobbyCreators.end()) {
        auto lobby = findLobbyByCode(lobbyCreator->second);
        if (lobby) {
            lobby->processIncomingMessage(connection, message);
            return;
        }
    }

    // Check if the connection is a player in a lobby
    auto playerInLobby = playersInLobbies.find(connection.id);

    if (playerInLobby != playersInLobbies.end()) {
        auto lobby = findLobbyByCode(playerInLobby->second);
        if (lobby) {
            lobby->processIncomingMessage(connection, message);
        }
    }
}

bool LobbyManager::isInLobby(const networking::Connection &connection) const {
    return playersInLobbies.find(connection.id) != playersInLobbies.end();
}

bool LobbyManager::isLobbyCreator(const networking::Connection &connection) const {
    return lobbyCreators.find(connection.id) != lobbyCreators.end();
}

bool LobbyManager::isDisplayNameUnique(const std::string &lobbyCode, const std::string &displayName) const {

    auto lobby = findLobbyByCode(lobbyCode);
    if (lobby) {
        auto players = lobby->getPlayers();
        return std::find_if(players.begin(), players.end(),
                            [&](const Player &p) { return p.getDisplayName() == displayName; }) == players.end();
    }

    return false;
}

void LobbyManager::updateLobbies() {
    for (auto it = lobbies.begin(); it != lobbies.end();) {
        if (it->second->getState() == Lobby::LobbyState::Finished) {
            // Get the list of players in the lobby
            auto players = it->second->getPlayers();
            auto lobbyCreator = it->second->getLobbyCreator();

            // Remove the lobby
            it = lobbies.erase(it);

            // Reinitialize players
            for (const auto &player : players) {
                // Remove player from playersInLobbies map
                playersInLobbies.erase(player.getConnection().id);

                // Call onConnect to display the welcome message
                server->sendToConnection("The game has ended. Thank you for playing!", player.getConnection());
                server->sendToConnection(generateWelcomeMessage(), player.getConnection());
            }

            lobbyCreators.erase(lobbyCreator.id);
            server->sendToConnection("Game Over!", lobbyCreator);
            server->sendToConnection(generateWelcomeMessage(), lobbyCreator);
        } else {
            it->second->update();
            ++it;
        }
    }
}

std::string LobbyManager::generateWelcomeMessage() {
    std::ostringstream oss;
    oss << "******************************************\n";
    oss << "*                                        *\n";
    oss << "* Welcome to the Social Gaming Platform! *\n";
    oss << "*                                        *\n";
    oss << "******************************************\n";
    oss << "*                                        *\n";
    oss << "*  Options:                              *\n";
    oss << "*    1. Create a new lobby               *\n";
    oss << "*    2. Join an existing lobby           *\n";
    oss << "*                                        *\n";
    oss << "******************************************\n";
    return oss.str();
}

bool LobbyManager::isLobbyCodeValid(const std::string &lobbyCode) const {

    return lobbies.find(lobbyCode) != lobbies.end();
}

bool LobbyManager::isWaitingForLobbyCode(const networking::Connection &connection) const {
    auto found = std::find(waitingForLobbyCode.begin(), waitingForLobbyCode.end(), connection);

    return found != waitingForLobbyCode.end();
}

void LobbyManager::addConnectionWaitingForLobbyCode(const networking::Connection &connection) {
    waitingForLobbyCode.push_back(connection);
}
