/////////////////////////////////////////////////////////////////////////////
//                         Single Threaded Networking
//
// This file is distributed under the MIT License. See the LICENSE file
// for details.
/////////////////////////////////////////////////////////////////////////////

#include "Server.h"

#include "optionsDisplay.h"

#include "GameConfigEdit.h"
#include "GameNameDisplayer.h"
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
std::unique_ptr<LobbyManager> lobbyManager;
std::shared_ptr<Messenger> messenger;
std::unique_ptr<GameSetupManager> gameSetupManager;

void onConnect(Connection c) {
    std::cout << "New connection found: " << c.id << "\n";
    clients.push_back(c);

    // Send options to the connected client
    messenger->sendToConnection(lobbyManager->generateWelcomeMessage(), c);
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

std::vector<GameCreators> listOfGameCreators;

MessageResult processMessages(Server &server, const std::deque<Message> &incoming) {
    std::ostringstream result;
    bool quit = false;
    for (auto &message : incoming) {
        auto &text = message.text;
        const auto &connection = message.connection;

        try {
            if (gameSetupManager->isGameCreator(connection)) {
                gameSetupManager->handleSetupMessage(connection, text);
            } else if (lobbyManager->isInLobby(connection) || lobbyManager->isLobbyCreator(connection)) {
                lobbyManager->routeMessage(connection, text);
            } else if (lobbyManager->isAwaitingDisplayName(connection)) {
                std::string displayName = text;
                lobbyManager->addPlayerToLobbyWithDisplayName(connection, displayName);
            } else if (std::stoi(text) == 1 && !lobbyManager->isWaitingForLobbyCode(connection)) {

                messenger->sendToConnection(
                    "Please select the a game you would like to create by entering its number \n", connection);
                messenger->sendToConnection(gameNameDisplayer(), connection);

                gameSetupManager->addGameCreator(connection);
            } else if (std::stoi(text) == 2 && !lobbyManager->isWaitingForLobbyCode(connection)) {
                messenger->sendToConnection("Enter the lobby code: ", connection);
                lobbyManager->addConnectionWaitingForLobbyCode(connection);
            } else if (lobbyManager->isWaitingForLobbyCode(connection)) {

                // Remove whitespace from the lobby code and check if it is valid
                std::string strippedText = text;
                strippedText.erase(std::remove_if(strippedText.begin(), strippedText.end(), ::isspace),
                                   strippedText.end());
                if (lobbyManager->isLobbyCodeValid(strippedText)) {
                    lobbyManager->addPlayerToLobby(strippedText, connection);
                } else {
                    messenger->sendToConnection("Invalid lobby code. Please try again.", connection);
                }
            }
        } catch (const std::invalid_argument &e) {
            messenger->sendToConnection("Invalid input. Please enter a valid number.", connection);
        } catch (const std::out_of_range &e) {
            messenger->sendToConnection("Input number is out of range. Please enter a valid number.", connection);
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
    gameSetupManager = std::make_unique<GameSetupManager>(messenger, lobbyManager.get());

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

        lobbyManager->updateLobbies();

        sleep(1);
    }

    return 0;
}
