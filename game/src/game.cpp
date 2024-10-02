#include "Client.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <ip address> <port>" << std::endl;
        return 1;
    }

    networking::Client client(argv[1], "../lib/web-socket-networking/webchat.html");
    bool done = false;
    while (!done && !client.isDisconnected()) {
        client.update();  

        // Check if the client received a message (i.e., the result from the server)
        std::string response = client.receive();
        if (!response.empty()) {
            // Display the result received from the server
            std::cout << "Received from server: " << response << std::endl;
        }

        // Get the player's choice (Rock, Paper, Scissors) and send it to the server
        std::string choice;
        std::cout << "Enter your choice (Rock, Paper, Scissors): ";
        std::cin >> choice;

        if (choice == "quit" || choice == "exit") {
            done = true;
        } else {
            client.send(choice);  // Send the player's choice to the server
        }
    }

    return 0;
}