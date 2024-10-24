/////////////////////////////////////////////////////////////////////////////
//                         Single Threaded Networking
//
// This file is distributed under the MIT License. See the LICENSE file
// for details.
/////////////////////////////////////////////////////////////////////////////


#include "Server.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <memory>
#include "GameManager.h"


using networking::Server;
using networking::Connection;
using networking::Message;


std::vector<Connection> clients;

//TODO: Tempory solution to allow the onConnect and onDisconnect to access the server object. refactor at a later date.
Server *server_ptr = nullptr;
GameManager gameManager;

// Create a new game and add it to the game manager
// Eventually this will create a game based on the user's selected game
std::shared_ptr<Game> createGame() {
  std::shared_ptr<Game> game = std::make_shared<Game>("Test");
  gameManager.addGame(game);
  return game;
}

void onConnect(Connection c) {
  std::cout << "New connection found: " << c.id << "\n";
  clients.push_back(c);
  
  // Send options to the connected client
  std::string welcomeMessage = "Welcome! Type 'create' to start a new game or 'join' to join an existing game.";
  std::deque<Message> outgoing;
  outgoing.push_back({c, welcomeMessage});
  server_ptr->send(outgoing); // Sending the message to the newly connected client
}


void
onDisconnect(Connection c) {
  std::cout << "Connection lost: " << c.id << "\n";
  auto eraseBegin = std::remove(std::begin(clients), std::end(clients), c);
  clients.erase(eraseBegin, std::end(clients));
}





struct MessageResult {
  std::string result;
  bool shouldShutdown;
};


MessageResult
processMessages(Server& server, const std::deque<Message>& incoming) {
  std::ostringstream result;
  bool quit = false;
  for (const auto& message : incoming) {
        if (message.text == "quit") {
            server.disconnect(message.connection);
        } else if (message.text == "shutdown") {
            std::cout << "Shutting down.\n";
            quit = true;
        } else if (message.text == "create") {

          // TODO: list of games



            std::shared_ptr<Game> game = createGame();
            std::deque<Message> outgoing;
            outgoing.push_back({message.connection, "New game created. Share the game code with others to join.\n"});
            outgoing.push_back({message.connection, "Game code: " + gameManager.getGameCode(game) + "\n"});
            server.send(outgoing);
        } else if (message.text == "join") {
            result << message.connection.id << "> Please enter 'join <game code>' to join a game.\n";
        } else if (message.text.rfind("join ", 0) == 0) {
            std::string gameCode = message.text.substr(5); // Extract the code after 'join '
            std::shared_ptr<Game> game = gameManager.getGame(gameCode);
            
            if (game) {
                Player player(message.connection, std::string("Player"));
                game->addPlayer(player);
                result << player.getDisplayName() << " joined game with code: " << gameCode << "\n";
                
                // Send a welcome message to the player
                std::string welcomeMessage = "Welcome to the game!";
                std::deque<Message> outgoing;
                outgoing.push_back({message.connection, welcomeMessage});
              

                // Optionally, notify other players in the game
                std::string notification = player.getDisplayName() + " has joined the game!";
                auto players = game->getPlayers();
                for (const auto& p : players) {
                    if (p.getConnection().id != message.connection.id) { // Don't notify the joining player
                        outgoing.push_back({p.getConnection(), notification});
                    }
                }
                server.send(outgoing);
            } else {
                result << message.connection.id << "> Game not found with code: " << gameCode << "\n";
            }
        } else {
            result << message.connection.id << "> " << message.text << "\n";
        }
    }
  return MessageResult{result.str(), quit};
}


std::deque<Message>
buildOutgoing(const std::string& log) {
  std::deque<Message> outgoing;
  for (auto client : clients) {
    outgoing.push_back({client, log});
  }
  return outgoing;
}


std::string
getHTTPMessage(const char* htmlLocation) {
  if (access(htmlLocation, R_OK ) != -1) {
    std::ifstream infile{htmlLocation};
    return std::string((std::istreambuf_iterator<char>(infile)),
                       std::istreambuf_iterator<char>());

  }

  std::cerr << "Unable to open HTML index file:\n"
            << htmlLocation << "\n";
  std::exit(-1);
}


int
main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "Usage:\n  " << argv[0] << " <port> <html response>\n"
              << "  e.g. " << argv[0] << " 4002 ./webchat.html\n";
    return 1;
  }

  const unsigned short port = std::stoi(argv[1]);
  Server server = {port, getHTTPMessage(argv[2]), onConnect, onDisconnect};
  server_ptr = &server;

 


  while (true) {
    bool errorWhileUpdating = false;
    try {
      server.update();
    } catch (std::exception& e) {
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

    sleep(1);
  }

  return 0;
}

