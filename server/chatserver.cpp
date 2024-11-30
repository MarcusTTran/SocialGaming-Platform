/////////////////////////////////////////////////////////////////////////////
//                         Single Threaded Networking
//
// This file is distributed under the MIT License. See the LICENSE file
// for details.
/////////////////////////////////////////////////////////////////////////////

#include "Server.h"

#include "optionsDisplay.h"

#include "LobbyManager.h"
#include "Messenger.h"
#include "NameResolver.h"
#include "GameParser.h"
#include "GameConfigEdit.h"
#include "GameNameDisplayer.h"
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

void onConnect(Connection c)
{
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

void onDisconnect(Connection c)
{
    std::cout << "Connection lost: " << c.id << "\n";
    auto eraseBegin = std::remove(std::begin(clients), std::end(clients), c);
    clients.erase(eraseBegin, std::end(clients));
}

struct MessageResult
{
    std::string result;
    bool shouldShutdown;
};

std::vector<GameCreators> listOfGameCreators;

MessageResult processMessages(Server &server, const std::deque<Message> &incoming)
{
    std::ostringstream result;
    bool quit = false;
    for (const auto &message : incoming)
    {
        const auto &text = message.text;
        const auto &connection = message.connection;
        auto currentMessageIsGameCreator = false;
        GameCreators *currentGameCreator = nullptr;
        for (auto &i : listOfGameCreators)
        {
            if (i.connectionID == message.connection.id)
            {
                currentMessageIsGameCreator = true;
                currentGameCreator = &i;
            }
        }

        if (lobbyManager->isAwaitingDisplayName(connection))
        { // Check if connection is awaiting display name
            std::string displayName = text;
            lobbyManager->addPlayerToLobbyWithDisplayName(connection, displayName);
        }
        else if (currentMessageIsGameCreator)
        { // if our current connection is in process of creating game.

            if (!currentGameCreator->chosenGameToEdit)
            {
                try
                {
                    auto number = std::stoi(message.text);
                    if (getConfigMap().find(number) == getConfigMap().end())
                    {
                        result << "Error, invalid entry, please enter an integer that exists within list of games: " << '\n';
                        break;
                    }
                    else
                    {
                        const std::string gameConfigPath = getConfigMap().at(number);
                    }
                }
                catch (const std::exception &e)
                {
                    // Catch any type of error-> std::exception
                    result << "Error, invalid entry, please enter an integer: " << e.what() << '\n';
                    break;
                }

                const std::string gameConfigPath = getConfigMap().at(std::stoi(message.text));
                // auto serverPtr = std::static_pointer_cast<IServer>(messenger);
                ParsedGameData parser(gameConfigPath, messenger);

                // // Game config now parses game selected by user.
                GameConfiguration config(parser);
                result << "You have chosen game " << message.text << " with config path: " << getConfigMap().at(std::stoi(message.text)) << '\n';
                result << "Do you wish to edit this games setup? or do you want to keep its default settings?" << '\n';
                result << "(Enter 'SAME' to choose default settings! )" << '\n';
                result << "(Enter 'CHANGE' to edit game settings! )" << '\n';
                currentGameCreator->chosenGameToEdit = true;
                currentGameCreator->adminGame = config;
                currentGameCreator->howManyGamesAdminHasToSet = config.getSetup().size() - 1; // the reason for minus 1 is for some reason the setup size is 1 bigger than it has to be.
                std::cout << "admin has this many games to set: " << currentGameCreator->howManyGamesAdminHasToSet << '\n';
                // messenger->sendToConnection(result.str(),connection);
            }
            else if (currentGameCreator->chosenGameToEdit)
            {

                if (message.text == "SAME" || currentGameCreator->howManyGamesAdminHasToSet == 0)
                {
                    if (currentGameCreator->howManyGamesAdminHasToSet == 0)
                    {
                        result << "Default settings chosen for game due to no setups existing in config! Game now being created. \n";
                    }
                    else
                    {
                        result << "Default settings chosen for game! Game now being created. \n";
                    }
                    // then game can be setup here as there is no edits to make.s
                    auto new_end = std::remove_if(
                        listOfGameCreators.begin(),
                        listOfGameCreators.end(),
                        [message](const GameCreators &creator)
                        {
                            return creator.connectionID == message.connection.id; // Replace 'id' with the actual field name.
                        });
                    listOfGameCreators.erase(new_end, listOfGameCreators.end());
                    
                }
                else if (message.text == "CHANGE" || currentGameCreator->chosenGameToEdit)
                {
                    auto configStatusResult = editingGameConfig(currentGameCreator->adminGame, currentGameCreator, message.text);

                    if (configStatusResult.status == EditState::Success)
                    {
                        result << configStatusResult.message;
                        if (currentGameCreator->editingSetup)
                        {
                            // just break as were currently editing things
                            break;
                        }
                        else
                        { // not editing a setup so were done and can move to next setup
                            currentGameCreator->gameConfigIterator++;
                            currentGameCreator->howManyGamesAdminHasToSet--;
                        }
                    }
                    if (configStatusResult.status == EditState::Error)
                    {
                    }
                    if (configStatusResult.status == EditState::Done || currentGameCreator->gameConfigIterator >= currentGameCreator->howManyGamesAdminHasToSet)
                    {
                        result << configStatusResult.message;
                        auto setups = currentGameCreator->adminGame.getSetup();
                        auto &setup = setups.at(0);
                        // if (setup.getRange().has_value())
                        // {
                        //     auto range = setup.getRange().value();
                        //     result << "NEWLY SET Range: (" << range.first << ", " << range.second << ")" << '\n';
                        // }

                        result << "NEWLY SET Choice: " << setup.chosenChoice << '\n';

                        auto new_end = std::remove_if(
                            listOfGameCreators.begin(),
                            listOfGameCreators.end(),
                            [message](const GameCreators &creator)
                            {
                                return creator.connectionID == message.connection.id; // Replace 'id' with the actual field name.
                            });
                        listOfGameCreators.erase(new_end, listOfGameCreators.end());

                        result << "Game now being created. \n";
                        break;
                    }
                    // going to need to figure out how to signal user is done editing game and then create said game.
                }
                else
                {
                    result << "Error invalid entry, please enter SAME or CHANGE to configure game settings. \n";
                }
            }
        }
        else if (text == "create")
        {

            // TODO: This is a temporary solution to create a game. This will be replaced with a user
            // selected game with a game configuration file.

            // assume we display this to the user
            result << gameNameDisplayer();
            result << "Please enter which game you would like to play by entering in its number (i.e 1): \n";

            // add a creator to the list as we may have multiple people creating a game at the same time.
            GameCreators creator;
            creator.connectionID = message.connection.id;
            creator.currentStepInGameConfigEdit = 0;
            listOfGameCreators.emplace_back(creator);

            // std::string gameName = "Rock Paper Scissors";
            // Game game(gameName);
            // lobbyManager->createLobby(game, connection);
        }
        else if (text.find("join") == 0)
        {
            if (text.length() <= 5)
            {
                std::string errorMessage = "Lobby code is missing. Please provide a valid lobby code.";
                std::deque<Message> outgoing;
                outgoing.push_back({connection, errorMessage});
                server.send(outgoing);
            }
            else
            {
                std::string lobbyCode = text.substr(5);
                lobbyManager->addPlayerToLobby(lobbyCode, connection);
            }
        }
        else
        {
            std::cout << "Routing message to lobby manager\n";
            lobbyManager->routeMessage(connection, text);
        }
    }

    return MessageResult{result.str(), quit};
}

std::deque<Message> buildOutgoing(const std::string &log)
{
    std::deque<Message> outgoing;
    for (auto client : clients)
    {
        outgoing.push_back({client, log});
    }
    return outgoing;
}

std::string getHTTPMessage(const char *htmlLocation)
{
    if (access(htmlLocation, R_OK) != -1)
    {
        std::ifstream infile{htmlLocation};
        return std::string((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    }

    std::cerr << "Unable to open HTML index file:\n"
              << htmlLocation << "\n";
    std::exit(-1);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage:\n  " << argv[0] << " <port> <html response>\n"
                  << "  e.g. " << argv[0] << " 4002 ./webchat.html\n";
        return 1;
    }

    const unsigned short port = std::stoi(argv[1]);
    Server server = {port, getHTTPMessage(argv[2]), onConnect, onDisconnect};
    server_ptr = &server;
    auto messenger = std::make_shared<Messenger>(server);
    lobbyManager = std::make_unique<LobbyManager>(messenger);

    while (true)
    {
        bool errorWhileUpdating = false;
        try
        {
            server.update();
        }
        catch (std::exception &e)
        {
            std::cerr << "Exception from Server update:\n"
                      << " " << e.what() << "\n\n";
            errorWhileUpdating = true;
        }

        const auto incoming = server.receive();
        const auto [log, shouldQuit] = processMessages(server, incoming);
        const auto outgoing = buildOutgoing(log);
        server.send(outgoing);

        if (shouldQuit || errorWhileUpdating)
        {
            break;
        }

        // TODO: need to loop through each lobby and update the game state

        for (const auto &[lobbyCode, lobby] : lobbyManager->getLobbies())
        {
            lobby->update();
        }

        sleep(1);
    }

    return 0;
}
