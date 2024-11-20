#include "Lobby.h"

Lobby::Lobby(const Game &game, std::shared_ptr<IServer> server, std::shared_ptr<networking::Connection> lobbyCreator,
             std::string lobbyCode)
    : game(std::make_unique<Game>(std::move(game))), server(server), lobbyCreator(lobbyCreator), lobbyCode(lobbyCode),
      state(LobbyState::Waiting) {
    server->sendToConnection("Lobby created with code: " + lobbyCode, *lobbyCreator);
}

Lobby::LobbyState Lobby::getState() { return state; }

void Lobby::addPlayer(const Player &player) {
    sendWelcomeMessage(player);

    players.push_back(player);
    sendCurrentListOfPlayers();
}

void Lobby::removePlayer(const Player &player) {
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

void Lobby::addMessage(const Message &message) { incomingMessages.push_back(message); }

void Lobby::processIncomingMessage(const networking::Connection &connection, const std::string &message) {

    // TODO: Think of incoming messages as events that trigger reesponses from incoming messages
    // need to refactor this to use a more event driven approach so that game logic can be implemented

    // check if the connection is the lobby creator
    if (connection.id == lobbyCreator->id) {
        if (message == "start") {
            sendToAll("Game starting!");
            state = LobbyState::InProgress;

            // This is where the game would start
        } else {
            std::string errorMessage = "Invalid command. Please type 'start' to start the game.";
            server->sendToConnection(errorMessage, connection);
            return;
        }
    }

    auto player = std::find_if(players.begin(), players.end(),
                               [&](const Player &p) { return p.getConnection().id == connection.id; });

    // This is where we would process the meesage from the player if the game was running.
    // The game would be responsible for processing the message and sending the appropriate
    // messages back to the players. For now, we will just echo the message back to the player.
    // Untill we have a more concrete implementation of the game logic.
    if (player != players.end()) {
        std::cout << "Player " << player->getDisplayName() << " said: " << message << std::endl;
        if (state == LobbyState::InProgress) {
            std::cout << "Game is in progress" << std::endl;
            // Add the incoming message to the queue of messages to be processed
            // Mesages will be processed on the next call to update()
            Message incomingMessage = {*player, message};
            addMessage(incomingMessage);

            if (message == "dump") {
                auto playersMap = getPlayersMap().asList();
                std::cout << "Dumping players map" << std::endl;
                for (auto &player : playersMap) {
                    server->sendMessageToPlayerMap("You said: " + message, player.asOrderedMap());
                }
            } else if (message == "leave") {
                removePlayer(*player);
            }

            else {
                sendToPlayer(*player, "You said: " + message);
            }
        }
    }
}

vector<Player> Lobby::getPlayers() const { return players; }

void Lobby::update() {}

DataValue Lobby::getPlayersMap() {
    std::vector<DataValue> playersMap;
    for (auto &player : players) {
        playersMap.push_back(DataValue(player.getMap(true)));
    }
    return DataValue(playersMap);
}