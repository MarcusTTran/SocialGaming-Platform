#include "Lobby.h"

Lobby::Lobby(std::unique_ptr<Game> game, std::shared_ptr<IServer> server,
             std::shared_ptr<networking::Connection> lobbyCreator, std::string lobbyCode)
    : game(std::move(game)), server(server), lobbyCreator(lobbyCreator), lobbyCode(lobbyCode),
      state(LobbyState::Waiting) {
    server->sendToConnection("Lobby created with code: " + lobbyCode, *lobbyCreator);

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

void Lobby::addPlayer(const Player &player) {

    if (state != LobbyState::Waiting) {
        server->sendToConnection("The game has already started. Please try again later.", player.getConnection());
        return;
    }
    sendWelcomeMessage(player);

    players.push_back(player);
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
    }
}

DataValue Lobby::getPlayersMap() {
    DataValue::OrderedMapType playersMap;

    for (auto &player : players) {
        playersMap.emplace(player.getDisplayName(), player.getMap(true));
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
