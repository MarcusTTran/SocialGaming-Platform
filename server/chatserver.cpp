/////////////////////////////////////////////////////////////////////////////
//                         Single Threaded Networking
//
// This file is distributed under the MIT License. See the LICENSE file
// for details.
/////////////////////////////////////////////////////////////////////////////

#include "Server.h"

#include "optionsDisplay.h"

#include "GameParser.h"
#include "LobbyManager.h"
#include "Messenger.h"
#include "NameResolver.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

using networking::Connection;
using networking::Message;
using networking::Server;

std::vector<Connection> clients;

// TODO: Tempory solution to allow the onConnect and onDisconnect to access the
// server object. refactor at a later date.
Server *server_ptr = nullptr;
std::shared_ptr<Messenger> messenger;
std::unique_ptr<LobbyManager> lobbyManager;

void onConnect(Connection c) {
    std::cout << "New connection found: " << c.id << "\n";
    clients.push_back(c);

    // Send options to the connected client
    std::string welcomeMessage =
        "Welcome! Type 'create' to start a new game or "
        "'join' followed by the code of the game you would like to join to join an existing game.";
    std::deque<Message> outgoing;
    outgoing.push_back({c, welcomeMessage});
    server_ptr->send(outgoing); // Sending the message to the newly connected client
}

void onDisconnect(Connection c) {
    std::cout << "Connection lost: " << c.id << "\n";
    auto eraseBegin = std::remove(std::begin(clients), std::end(clients), c);
    clients.erase(eraseBegin, std::end(clients));
}

struct MessageResult {
    std::string result;
    bool shouldShutdown;
};

MessageResult processMessages(Server &server, const std::deque<Message> &incoming) {
    std::ostringstream result;
    bool quit = false;
    for (const auto &message : incoming) {
        const auto &text = message.text;
        const auto &connection = message.connection;

        if (lobbyManager->isAwaitingDisplayName(connection)) { // Check if connection is awaiting display name
            std::string displayName = text;
            lobbyManager->addPlayerToLobbyWithDisplayName(connection, displayName);
        } else if (text == "create") {

            // TODO: This is a temporary solution to create a game. This will be replaced with a user
            // selected game with a game configuration file.

            const std::string gameConfigFile = "../config/minimal.game";
            ParsedGameData gameData(gameConfigFile, messenger);
            std::cout << "Game name: " << gameData.getGameName() << "\n";
            // std::cout << "Num rules: " << gameData.getRules().size() << "\n";
            std::unique_ptr<Game> game = std::make_unique<Game>(gameData, gameData.getGameName());
            lobbyManager->createLobby(std::move(game), connection);
        } else if (text.find("join") == 0) {
            if (text.length() <= 5) {
                std::string errorMessage = "Lobby code is missing. Please provide a valid lobby code.";
                std::deque<Message> outgoing;
                outgoing.push_back({connection, errorMessage});
                server.send(outgoing);
            } else {
                std::string lobbyCode = text.substr(5);
                lobbyManager->addPlayerToLobby(lobbyCode, connection);
            }
        } else {
            std::cout << "Routing message to lobby manager\n";
            lobbyManager->routeMessage(connection, text);
        }
    }

    return MessageResult{result.str(), quit};
}

std::deque<Message> buildOutgoing(const std::string &log) {
    std::deque<Message> outgoing;
    for (auto client : clients) {
        outgoing.push_back({client, log});
    }
    return outgoing;
}

std::string getHTTPMessage(const char *htmlLocation) {
    if (access(htmlLocation, R_OK) != -1) {
        std::ifstream infile{htmlLocation};
        return std::string((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    }

    std::cerr << "Unable to open HTML index file:\n" << htmlLocation << "\n";
    std::exit(-1);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage:\n  " << argv[0] << " <port> <html response>\n"
                  << "  e.g. " << argv[0] << " 4002 ./webchat.html\n";
        return 1;
    }

    const unsigned short port = std::stoi(argv[1]);
    Server server = {port, getHTTPMessage(argv[2]), onConnect, onDisconnect};
    server_ptr = &server;
    messenger = std::make_shared<Messenger>(server);
    lobbyManager = std::make_unique<LobbyManager>(messenger);

    while (true) {
        bool errorWhileUpdating = false;
        try {
            server.update();
        } catch (std::exception &e) {
            std::cerr << "Exception from Server update:\n"
                      << " " << e.what() << "\n\n";
            errorWhileUpdating = true;
        }

        const auto incoming = server.receive();
        const auto [log, shouldQuit] = processMessages(server, incoming);
        const auto outgoing = buildOutgoing(log);
        server.send(outgoing);

        if (shouldQuit || errorWhileUpdating) {
            break;
        }

        // TODO: need to loop through each lobby and update the game state

        for (const auto &[lobbyCode, lobby] : lobbyManager->getLobbies()) {
            lobby->update();
        }

        sleep(1);
    }

    return 0;
}
