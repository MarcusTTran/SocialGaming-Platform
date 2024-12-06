#include "GameConfigEdit.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

bool GameSetupManager::isGameCreator(const networking::Connection &connection) const {
    return std::any_of(listOfGameCreators.begin(), listOfGameCreators.end(),
                       [&connection](const GameCreators &creator) { return creator.connectionID == connection.id; });
}

void GameSetupManager::handleSetupMessage(const networking::Connection &connection, const std::string &message) {
    auto it =
        std::find_if(listOfGameCreators.begin(), listOfGameCreators.end(),
                     [&connection](const GameCreators &creator) { return creator.connectionID == connection.id; });

    if (it == listOfGameCreators.end()) {
        return;
    }

    GameCreators *currentGameCreator = &(*it);

    if (!currentGameCreator->chosenGameToEdit) {
        std::string gameConfigPath;
        int number;
        try {
            number = std::stoi(message);
            std::cout << "Number: " << number << std::endl;
            if (getConfigMap().find(number) == getConfigMap().end()) {
                messenger->sendToConnection(
                    "Error, invalid entry, please enter an integer that exists within list of games.", connection);
                return;
            } else {
                gameConfigPath = getConfigMap().at(number);
                std::cout << "Game Config Path: " << gameConfigPath << std::endl;
            }
        } catch (const std::exception &e) {
            std::cerr << "Error parsing message: " << e.what() << std::endl;
            messenger->sendToConnection("Error, invalid entry, please enter an integer: " + std::string(e.what()),
                                        connection);
            return;
        }

        std::shared_ptr<ParsedGameData> parser = std::make_shared<ParsedGameData>(gameConfigPath, messenger);
        GameConfiguration config(*parser);
        currentGameCreator->chosenGameToEdit = true;
        currentGameCreator->adminGame = config;
        currentGameCreator->parserObject = parser;
        currentGameCreator->howManyGamesAdminHasToSet = config.getSetup().size() - 1;
        messenger->sendToConnection(
            "You have chosen game " + message + " with config path: " + getConfigMap().at(number), connection);
        messenger->sendToConnection(
            "Do you wish to edit this game's setup? or do you want to keep its default settings?\n(Enter 'SAME' to "
            "choose default settings! )\n(Enter 'CHANGE' to edit game settings! )",
            connection);
    } else if (currentGameCreator->chosenGameToEdit) {
        if (message == "SAME" || currentGameCreator->howManyGamesAdminHasToSet == 0) {
            std::string gameName = currentGameCreator->adminGame.getGameName().getName();
            std::unique_ptr<Game> game = std::make_unique<Game>(*currentGameCreator->parserObject, gameName);
            listOfGameCreators.erase(it);
            lobbyManager->createLobby(std::move(game), connection);
        } else if (message == "CHANGE" || currentGameCreator->chosenGameToEdit) {
            auto configStatusResult = editingGameConfig(currentGameCreator->adminGame, currentGameCreator, message);
            if (configStatusResult.status == EditState::Success) {
                if (currentGameCreator->editingSetup) {
                    return;
                } else {
                    currentGameCreator->gameConfigIterator++;
                    currentGameCreator->howManyGamesAdminHasToSet--;
                }
            }
            if (configStatusResult.status == EditState::Done ||
                currentGameCreator->gameConfigIterator >= currentGameCreator->howManyGamesAdminHasToSet) {
                std::string gameName = currentGameCreator->adminGame.getGameName().getName();
                std::unique_ptr<Game> game = std::make_unique<Game>(*currentGameCreator->parserObject, gameName);
                listOfGameCreators.erase(it);
                lobbyManager->createLobby(std::move(game), connection);
            }
        } else {
            messenger->sendToConnection("Error invalid entry, please enter SAME or CHANGE to configure game settings.",
                                        connection);
        }
    }
}

void GameSetupManager::addGameCreator(const networking::Connection &connection) {
    GameCreators creator;
    creator.connectionID = connection.id;
    creator.currentStepInGameConfigEdit = 0;
    listOfGameCreators.emplace_back(creator);
}

void GameSetupManager::removeGameCreator(const networking::Connection &connection) {
    auto it =
        std::remove_if(listOfGameCreators.begin(), listOfGameCreators.end(),
                       [&connection](const GameCreators &creator) { return creator.connectionID == connection.id; });
    listOfGameCreators.erase(it, listOfGameCreators.end());
}

ConfigEditResult GameSetupManager::editingGameConfig(GameConfiguration &config, GameCreators *currentGameCreator,
                                                     std::string adminMessage) {
    std::ostringstream result;
    auto &setups = config.getSetup();
    auto &setup = setups.at(currentGameCreator->gameConfigIterator);

    if (currentGameCreator->gameConfigIterator <= currentGameCreator->howManyGamesAdminHasToSet &&
        !currentGameCreator->editingSetup) {
        result << "Name: " << config.getGameName().getName() << '\n';
        result << "Player Range: " << config.getPlayerRange().first << ", " << config.getPlayerRange().second << '\n';
        result << "Audience: " << config.hasAudience() << '\n';
        result << "List of Game Setups Below: \n";
        result << " \n";
        if (setup.kind.has_value()) {
            result << "Setup Kind: " << *setup.kind << '\n';
        }
        if (setup.prompt.has_value()) {
            result << "Setup prompt: " << *setup.prompt << '\n';
        }
        if (setup.getRange().has_value()) {
            auto range = setup.getRange().value();
            result << "Range: (" << range.first << ", " << range.second << ")" << '\n';
        }
        if (setup.getChoices().has_value()) {
            auto choices = setup.getChoices().value();
            result << "Choices:" << '\n';
            for (const auto &[key, description] : choices) {
                result << "  " << key << ": " << description << '\n';
            }
        }
        if (setup.getDefault().has_value()) {
            auto defaultValue = setup.getDefault().value();
            result << "Default Value:" << '\n';
            for (const auto &[key, value] : defaultValue) {
                result << "  " << key << ": " << value.getType() << '\n';
            }
        }
        result << "Please enter kind and your desired configuration (I.e for range 'integer 1-3', for choice 'enum 1(1 "
                  "stands for first game starting from top to bottom.):', for true/false 'boolean false', for prompt "
                  "'prompt messageyouwanthere') : \n";
        currentGameCreator->editingSetup = true;
        result << " \n";
        messenger->sendToConnection(result.str(), networking::Connection{currentGameCreator->connectionID});
        ConfigEditResult msg;
        msg.message = result.str();
        msg.status = EditState::Success;
        return msg;
    } else if (currentGameCreator->editingSetup) {
        auto editResult = parsingEditInput(adminMessage, setup, config);
        ConfigEditResult msg;
        msg.message = result.str();
        if (editResult.editSuccessValue) {
            msg.status = EditState::Success;
            currentGameCreator->editingSetup = false;
        } else {
            msg.status = EditState::Error;
        }
        return msg;
    } else {
        result << "no more setups to edit! \n";
        ConfigEditResult msg;
        msg.message = result.str();
        msg.status = EditState::Done;
        return msg;
    }
}

ParsedEditInput GameSetupManager::parsingEditInput(const std::string &message, GameConfiguration::Setup &setup,
                                                   GameConfiguration &config) {
    ParsedEditInput result;
    std::istringstream stream(message);
    stream >> result.kind;
    std::getline(stream, result.value);
    if (result.kind == "integer") {
        if (result.value.find('-') == std::string::npos) {
            if (setup.getDefault().has_value()) {
                auto defaultValue = setup.getDefault().value();
                for (auto &[key, value] : defaultValue) {
                    value = result.value;
                    std::cout << "SET DEFAULT VALUE TO NOW BE : " << value << '\n';
                }
            }
        } else {
            char dash = '-';
            std::istringstream range(result.value);
            range >> result.start >> dash >> result.end;
            if (setup.getRange().has_value()) {
                setup.range.value().first = result.start;
                setup.range.value().second = result.end;
                std::cout << "Range SET TO NOW BE: (" << setup.range.value().first << ", " << setup.range.value().second
                          << ")" << '\n';
            }
            return result;
        }
    } else if (result.kind == "enum") {
        int index = std::stoi(result.value) - 1;
        auto &holder = setup.choices.value().at(index);
        setup.chosenChoice = holder.second;
        return result;
    } else if (result.kind == "boolean") {
    } else if (result.kind == "prompt") {
    } else {
        result.editSuccessValue = false;
        return result;
    }
}