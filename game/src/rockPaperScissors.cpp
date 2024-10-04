// author: kwa132, Mtt8

#include "rockPaperScissors.h"

RockPaperScissorsGame::RockPaperScissorsGame(GameConfig gf, int round) : config(gf), maxRound(round)
    , size_of_player(0){}

string RockPaperScissorsGame::determineWinner(){
    auto player1 = players.begin();
    auto player2 = next(player1);

    const string p1_choice = toLowerCase(player1->second.getChoice());
    const string p2_choice = toLowerCase(player2->second.getChoice());

    string result = "";
    if (p1_choice == p2_choice) {
        result = "It's a tie!\n";
    } else if (rules[p1_choice] == p2_choice) {
        result = "Player " + to_string(player1->second.getId()) + " wins!\n";
    } else {
        result = "Player " + to_string(player2->second.getId()) + " wins!\n";
    }

    return result;
}

void RockPaperScissorsGame::waitInRoom(networking::Connection c) {
    // busy wait
    while(players.size() > 2){}
    waitingList.pop();
}

void RockPaperScissorsGame::sendingMessage(networking::Server& server, networking::Connection connection, const string& text) const{
    networking::Message message{connection, text};
    server.send({message});
}

bool RockPaperScissorsGame::bothPlayerHaveChosen(){
    auto it = players.begin();
    return !it->second.getChoice().empty() && !next(it)->second.getChoice().empty();
}

string RockPaperScissorsGame::toLowerCase(const string& str){
    string lowerStr = str;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

bool RockPaperScissorsGame::isValidChoice(const string& choice){
    string lowerChoice = toLowerCase(choice);
    return (lowerChoice == "rock" || lowerChoice == "paper" || lowerChoice == "scissors");
}

void RockPaperScissorsGame::processPlayerChoice(networking::Connection connection, 
    const string& choice, networking::Server& server){
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

    if (bothPlayerHaveChosen()) {
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

void RockPaperScissorsGame::resetPlayerChoices(){
    for (auto& [conn, player] : players) {
        player.make_choice("");  
    }
}

string RockPaperScissorsGame::getGameName() const{
    return "Rock-Paper-Scissors";
}