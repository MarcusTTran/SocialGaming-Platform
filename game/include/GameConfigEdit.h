#pragma once

#include "GameConfiguration.h"
#include "GameNameDisplayer.h"
#include "GameParser.h"
#include "LobbyManager.h"
#include "Messenger.h"
#include "game.h"

struct GameCreators {
    uintptr_t connectionID;
    GameConfiguration adminGame;
    std::shared_ptr<ParsedGameData> parserObject;
    int howManyGamesAdminHasToSet = 0; // this will be used to manage state of where user is in editing process.
    int gameConfigIterator =
        0; // will update in tandem of howManyGamesAdminHasToSet in order to access config setup vector.
    bool choseDefaultSettings = false;
    bool chosenGameToEdit = false;
    bool editingSetup = false;
    int currentStepInGameConfigEdit; // there are 5 if statement checks as described in main.cpp to check for values for
                                     // setups, this will hold where we are in that process.
};

enum class EditState {
    Success, // user successfully edited a game setup, move onto next one based on this signal.
    Error,   // user unsuccessfully edited a game setup, retry this setup step based on this signal.
    Done     // no more setups to edit, user is done.
};

struct ConfigEditResult {
    EditState status;
    std::string message;
};
struct ParsedEditInput {
    std::string kind;
    std::string value;
    int start; // ranges
    int end;   // ranges
    bool editSuccessValue = true;
};

class GameSetupManager {
public:
    GameSetupManager(std::shared_ptr<Messenger> messenger, LobbyManager *lobbyManager)
        : messenger(messenger), lobbyManager(lobbyManager) {}

    bool isGameCreator(const networking::Connection &connection) const;
    void handleSetupMessage(const networking::Connection &connection, const std::string &message);
    void addGameCreator(const networking::Connection &connection);
    void removeGameCreator(const networking::Connection &connection);

private:
    std::vector<GameCreators> listOfGameCreators;
    std::shared_ptr<Messenger> messenger;
    LobbyManager *lobbyManager;

    ConfigEditResult editingGameConfig(GameConfiguration &, GameCreators *, std::string);
    ParsedEditInput parsingEditInput(const std::string &, GameConfiguration::Setup &, GameConfiguration &,GameCreators *currentGameCreator);
};
