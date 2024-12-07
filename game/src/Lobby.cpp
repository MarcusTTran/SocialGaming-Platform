#include "Lobby.h"

Lobby::Lobby(std::unique_ptr<Game> game, std::shared_ptr<IServer> server,
             std::shared_ptr<networking::Connection> lobbyCreator, std::string lobbyCode)
    : game(std::move(game)), server(server), lobbyCreator(lobbyCreator), lobbyCode(lobbyCode),
      state(LobbyState::Waiting) {
    server->sendToConnection(generateLobbyCreatedMessage(lobbyCode), *lobbyCreator);

    // Register event handlers
    eventHandlers["start"] = [this](const networking::Connection &connection, const std::string &message) {
        handleStartEvent(connection, message);
    };
    eventHandlers["dump"] = [this](const networking::Connection &connection, const std::string &message) {
        handleDumpEvent(connection, message);
    };
    eventHandlers["leave"] = [this](const networking::Connection &connection, const std::string &message) {
        handleLeaveEvent(connection, message);
    };
}

Lobby::LobbyState Lobby::getState() { return state; }

void Lobby::addPlayer(Player &player) {

    if (state != LobbyState::Waiting) {
        server->sendToConnection("The game has already started. Please try again later.", player.getConnection());
        return;
    }

    auto maxPlayers = game->maxPlayers();
    auto hasAudience = game->hasAudience();

    if (players.size() >= maxPlayers && !hasAudience) {
        server->sendToConnection("The lobby is full. Please try again later", player.getConnection());
        return;
    }

    if (players.size() >= maxPlayers && hasAudience) {
        server->sendToConnection("The lobby is full. Joining as an audience member", player.getConnection());
        player.addPerVariableMap(game->getPerAudienceMap(), false);

        // For testing purposes
        std::cout << "Per audience map size : " << game->getPerAudienceMap().size() << std::endl;

        // print out per audience map
        auto audienceMap = player.getMap(false);
        for (const auto &pair : audienceMap) {
            std::cout << pair.first << " : " << pair.second << std::endl;
        }

    } else {
        player.addPerVariableMap(game->getPerPlayerMap(), true);

        // For testing purposes
        std::cout << "Per player map size : " << game->getPerPlayerMap().size() << std::endl;

        // print out per player map
        auto playerMap = player.getMap(true);
        for (const auto &pair : playerMap) {
            std::cout << pair.first << " : " << pair.second << std::endl;
        }
    }

    players.push_back(player);
    sendWelcomeMessage(player);
    sendCurrentListOfPlayers();
}

void Lobby::removePlayer(const Player &player) {

    if (state == LobbyState::InProgress) {
        server->sendToConnection("You cannot leave the lobby once the game has started.", player.getConnection());
        return;
    }
    sendToPlayer(player, "You have left the lobby.");
    players.erase(std::remove(players.begin(), players.end(), player), players.end());
    sendToAll(player.getDisplayName() + " has left the lobby.");
    sendCurrentListOfPlayers();
}

void Lobby::sendToAll(const std::string &message) {
    std::deque<networking::Message> outgoing;

    for (const auto &player : players) {
        outgoing.push_back({player.getConnection(), message});
    }

    server->sendMessages(outgoing);
}

void Lobby::sendToPlayer(const Player &player, const std::string &message) {
    server->sendToConnection(message, player.getConnection());
}

void Lobby::sendWelcomeMessage(const Player &player) {
    std::string welcomeMessage = "Welcome to the lobby, " + player.getDisplayName() + "!";
    welcomeMessage += " The game is " + game->getGameName();

    server->sendToConnection(welcomeMessage, player.getConnection());
}

void Lobby::sendCurrentListOfPlayers() {
    std::string message = "Current players in the lobby: ";
    for (const auto &player : players) {
        message += player.getDisplayName() + ", ";
    }
    server->sendToConnection(message, *lobbyCreator);
}

void Lobby::addMessage(const networking::Message &message) { incomingMessages.push_back(message); }

void Lobby::processIncomingMessage(const networking::Connection &connection, const std::string &message) {

    auto handler = eventHandlers.find(message);
    if (handler != eventHandlers.end()) {
        handler->second(connection, message);
    } else {

        if (state == LobbyState::Waiting) {
            return;

        } else {
            auto player = std::find_if(players.begin(), players.end(),
                                       [&](const Player &p) { return p.getConnection().id == connection.id; });
            if (player != players.end()) {
                addMessage(networking::Message{connection, message});
            }
        }
    }
}

vector<Player> Lobby::getPlayers() const { return players; }

void Lobby::update() {

    if (state == LobbyState::InProgress) {
        game->insertIncomingMessages(incomingMessages);
        game->updateGame();
        incomingMessages.clear();
        if (game->isGameDone()) {
            state = LobbyState::Finished;
        }
    }
}

DataValue Lobby::getPlayersMap() {
    std::vector<DataValue> playersMap;

    for (auto &player : players) {
        playersMap.push_back(DataValue(player.getMap(true)));
    }
    return DataValue(playersMap);
}

void Lobby::handleStartEvent(const networking::Connection &connection, const std::string &message) {
    if (connection.id == lobbyCreator->id) {
        sendToAll("Game starting!");
        auto playersMap = getPlayersMap();
        state = LobbyState::InProgress;
        game->startGame(playersMap);

    } else {
        std::string errorMessage = "Only the lobby creator can start the game.";
        server->sendToConnection(errorMessage, connection);
    }
}

void Lobby::handleDumpEvent(const networking::Connection &connection, const std::string &message) {
    auto playersMap = getPlayersMap().asOrderedMap();
    for (auto &player : playersMap) {
        server->sendMessageToPlayerMap("You said: " + message, player.second.asOrderedMap());
    }
}

void Lobby::handleLeaveEvent(const networking::Connection &connection, const std::string &message) {
    auto player = std::find_if(players.begin(), players.end(),
                               [&](const Player &p) { return p.getConnection().id == connection.id; });
    if (player != players.end()) {
        removePlayer(*player);
    }
}

void Lobby::handleUnknownEvent(const networking::Connection &connection, const std::string &message) {
    std::string errorMessage = "Unknown command: " + message;
    server->sendToConnection(errorMessage, connection);
}

networking::Connection &Lobby::getLobbyCreator() const { return *lobbyCreator; }

std::string Lobby::generateLobbyCreatedMessage(const std::string &lobbyCode) const {
    std::ostringstream oss;
    oss << "******************************************\n";
    oss << "*                                        *\n";
    oss << "*             Lobby Created!             *\n";
    oss << "*                                        *\n";
    oss << "******************************************\n";
    oss << "*                                        *\n";
    oss << "*        Your lobby code is:             *\n";
    oss << "*        " << lobbyCode << "                          *\n";
    oss << "*                                        *\n";
    oss << "*    Share this code with others to      *\n";
    oss << "*    invite them to your lobby.          *\n";
    oss << "*                                        *\n";
    oss << "******************************************\n";
    return oss.str();
}
