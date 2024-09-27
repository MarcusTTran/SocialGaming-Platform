#include "Server.h"
#include "Player.h"
#include <iostream>
#include <string>
#include <map>
#include <deque>
#include <queue>
#include <algorithm>
#include <fstream>

// Function to load the HTML content
string getHTTPMessage(const char* htmlLocation) {
    ifstream infile{htmlLocation};
    if (!infile) {
        cerr << "Unable to open HTML index file: " << htmlLocation << "\n";
        exit(-1);
    }
    return string{istreambuf_iterator<char>(infile),
                       istreambuf_iterator<char>()};
}

void waitInRoom(networking::Connection);
void sendingMessage(networking::Server&, networking::Connection, const string&);

struct ConnectionComparator {
    bool operator()(const networking::Connection& lhs, const networking::Connection& rhs) const {
        return lhs.id < rhs.id; 
    }
};

int size_of_player = 0;
map<networking::Connection, Player, ConnectionComparator> players;
queue<networking::Connection> waitingList;

void onConnect(networking::Server& server, networking::Connection c) {
    cout << "New connection: " << c.id << endl;
    string greeting = "Welcome to play the game: Rock-Paper-Scissors.\n";
    greeting += "You are: player " + to_string(size_of_player + 1) + "\n";
    
    sendingMessage(server, c, greeting);

    // If there are already 2 players, add the player to the waiting list
    if(players.size() >= 2) {
        sendingMessage(server, c, "The game is full. You are in the waiting room.\n");
        waitingList.push(c);
    } else {
        // Add player to the active game
        size_of_player++;
        players.emplace(c, Player(c, 3, size_of_player));
        sendingMessage(server, c, "You have joined the game.\n");
    }
}

void onDisconnect(networking::Connection c) {
    cout << "Connection lost: " << c.id << endl;
    size_of_player--;
}

void waitInRoom(networking::Connection c) {
    // busy wait
    while(players.size() > 2){}
    waitingList.pop();
}

string toLowerCase(const string& str) {
    string lowerStr = str;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

bool bothPlayersHaveChosen() {
    auto it = players.begin();
    return !it->second.getChoice().empty() && !next(it)->second.getChoice().empty();
}

// Function to validate player's choice
bool isValidChoice(const string& choice) {
    string lowerChoice = toLowerCase(choice);
    return (lowerChoice == "rock" || lowerChoice == "paper" || lowerChoice == "scissors");
}

// Function to determine the winner of Rock-Paper-Scissors
string determineWinner() {
    auto player1 = players.begin();
    auto player2 = next(player1);

    const string p1_choice = toLowerCase(player1->second.getChoice());
    const string p2_choice = toLowerCase(player2->second.getChoice());

    string result = "";
    if (p1_choice == p2_choice) {
        result = "It's a tie!\n";
    } else if ((p1_choice == "rock" && p2_choice == "scissors") ||
               (p1_choice == "paper" && p2_choice == "rock") ||
               (p1_choice == "scissors" && p2_choice == "paper")) {
        result = "Player " + to_string(player1->second.getId()) + " wins!\n";
    } else {
        result = "Player " + to_string(player2->second.getId()) + " wins!\n";
    }

    return result;
}

// Function to reset player choices after each round
void resetPlayerChoices() {
    for (auto& [conn, player] : players) {
        player.make_choice("");  
    }
}

// Function to send message from server to client
void sendingMessage(networking::Server& server, networking::Connection connection, const string& text) {
    networking::Message message{connection, text};
    server.send({message});
}

// Function to handle player choices
void processPlayerChoice(networking::Connection connection, const string& choice, networking::Server& server) {
    if (players.find(connection) == players.end()) {
        sendingMessage(server, connection, "You are currently in the waiting room.\n");
        return;
    }

    auto& player = players[connection];

    if (player.roundGetter() == 0) {
        sendingMessage(server, connection, "Player" + to_string(player.getId()) + 
        " has no more rounds left. Please rejoin to play again.\n");
        players.erase(connection);
        size_of_player--;
        
        // If someone is in the waiting room, let them join the game
        if (!waitingList.empty()) {
            networking::Connection nextPlayer = waitingList.front();
            waitingList.pop();
            players.emplace(nextPlayer, Player(nextPlayer, 3, ++size_of_player));
            sendingMessage(server, nextPlayer, "You have joined the game from the waiting room.\n");
        }
        return;
    }

    if (!isValidChoice(choice)) {
        string invalidMessage = "Invalid choice! Please enter Rock, Paper, or Scissors.\n";
        cout << "Player " << connection.id << " typed an invalid choice: " << choice << "\n";
        sendingMessage(server, connection, invalidMessage);
        return;  
    }

    player.make_choice(toLowerCase(choice));
    player.one_round_drop();
    sendingMessage(server, connection, "Your choice is: " + choice + ".\n");

    if (bothPlayersHaveChosen()) {
        string result = determineWinner();
        cout << result << endl;

        auto p1 = players.begin();
        auto p2 = next(p1);
        sendingMessage(server, p1->first, result);
        sendingMessage(server, p2->first, result);

        resetPlayerChoices();  
    } else {
        cout << "Waiting for the other player to make a choice...\n";
    }
}

// Process incoming messages from players
void processIncomingMessage(networking::Server& server, const deque<networking::Message>& incoming) {
    for (const auto& message : incoming) {
        processPlayerChoice(message.connection, message.text, server);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <port> <html file>\n";
        return 1;
    }

    // Create a server that listens on the specified port
    unsigned short port = stoi(argv[1]);

    // Load the HTML file content
    string htmlContent = getHTTPMessage(argv[2]);

    // Create a server instance
    networking::Server server(port, htmlContent, 
        [&server](networking::Connection c) { onConnect(server, c); },
        onDisconnect);

    while (true) {
        try {
            // Update the server to process incoming connections and messages
            server.update();
            
            // Retrieve messages from the clients
            deque<networking::Message> incoming = server.receive();

            // Process the messages
            processIncomingMessage(server, incoming);

        } catch (const exception& e) {
            cerr << "Server error: " << e.what() << endl;
        }
    }

    return 0;
}
