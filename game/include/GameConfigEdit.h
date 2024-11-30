#pragma once

#include "GameParser.h"
#include "GameConfiguration.h"
#include "game.h"


struct GameCreators
{
    uintptr_t connectionID;
    GameConfiguration adminGame;
    std::shared_ptr<ParsedGameData> parserObject;
    int howManyGamesAdminHasToSet = 0;// this will be used to manage state of where user is in editing process.
    int gameConfigIterator = 0; // will update in tandem of howManyGamesAdminHasToSet in order to access config setup vector.
    bool choseDefaultSettings = false;
    bool chosenGameToEdit = false;
    bool editingSetup = false;
    int currentStepInGameConfigEdit; // there are 5 if statement checks as described in main.cpp to check for values for setups, this will hold where we are in that process.
};


enum class EditState{
    Success,// user successfully edited a game setup, move onto next one based on this signal.
    Error, // user unsuccessfully edited a game setup, retry this setup step based on this signal.
    Done // no more setups to edit, user is done.
};

struct ConfigEditResult{
    EditState status;
    std::string message;
};
struct ParsedEditInput
{
    std::string kind;
    std::string value;
    int start; // ranges
    int end;   // ranges
    bool editSuccessValue = true;
};


ConfigEditResult editingGameConfig(GameConfiguration&,GameCreators*,std::string );
ParsedEditInput parsingEditInput(const std::string &,GameConfiguration::Setup &, GameConfiguration &);