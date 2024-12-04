#include <iostream>
#include <memory>
#include <unordered_map>
#include <string>
#include "optionsDisplay.h"
#include "GameParser.h"
#include "game.h"
#include "GameNameDisplayer.h"
#include "Messenger.h"

class MockMessenger : public IServer {
public:
    MockMessenger() = default;

    void broadcast(const std::string &message, const std::vector<networking::Connection> &connections) override {

    }
    void sendToConnection(const std::string &message, const networking::Connection &connection) override {

    }
    void sendMessages(const std::deque<networking::Message> &messages) override {

    }
    void sendMessageToPlayerMap(const std::string &message, const DataValue::OrderedMapType &playerMap) override {

    }

};


int main()
{   
    // TODO: fix this it is not useable right now
    std::shared_ptr<MockMessenger> mockMessenger = std::make_shared <MockMessenger>();
    ParsedGameData parser = ParsedGameData("../373-24-fahsa/config/config.game", mockMessenger);
    // Call methods on the parser as needed
    std::string gameName = parser.getGameName();
    std::cout << "Game name: " << gameName << std::endl;
    return 0;
}




// try
    // {
    //     // Test ConfigurationOptions
    //     // std::unordered_map<std::string, std::string> enumChoices = {
    //     //     {"fast", "A quick round with friends"},
    //     //     {"standard", "Standard play"},
    //     //     {"long", "A marathon battle against former friends"}};
    //     // namePlayerAudience test = {"Rock Paper Scissors", std::make_pair(1, 10), true};
    //     // std::string filename = "../config/rockPaperScissors.game";
    //     // ParsedGameData parser(filename);
    //     // // Game game(parser, "Rock-Paper-Scissors");
    //     // GameConfiguration config(parser);

    //     // auto enumConfig = createConfigurationOptions(config);
    //     // std::cout << enumConfig->displayOptions() << '\n';

    //     // auto boolConfig = createConfigurationOptions(test, "boolean", "Color is blue", nullptr, 0, 0);
    //     // std::cout << boolConfig->displayOptions() << '\n';

    //     // auto intConfig = createConfigurationOptions(&config);
    //     // std::cout << intConfig->displayOptions() << '\n';
    //     // // Test Invalid
    //     // auto invalidConfig = createConfigurationOptions(test, "invalid", "Number of players", nullptr, 1, 10);
    //     // std::cout << intConfig->displayOptions() << '\n';

    // }
    // catch (const std::exception &e)
    // {
    //     std::cerr << "Error: " << e.what() << '\n';
    // }

    // std::cout << gameNameDisplayer();
    //     for(auto i: getConfigMap()){
    //         std::cout << "Key: " << i.first << " value: " << i.second << '\n';
    //     }
