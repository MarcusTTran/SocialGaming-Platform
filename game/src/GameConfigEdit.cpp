
#include "GameConfigEdit.h"
#include <iostream>
#include <string>

ConfigEditResult editingGameConfig(GameConfiguration &config, GameCreators *currentGameCreator, std::string adminMessage)
{
    std::ostringstream result;

    auto &setups = config.getSetup();
    auto &setup = setups.at(currentGameCreator->gameConfigIterator);

    if (currentGameCreator->gameConfigIterator <= currentGameCreator->howManyGamesAdminHasToSet && !currentGameCreator->editingSetup)
    {
        result << "Name: " << config.getGameName().getName() << '\n';
        result << "Player Range: " << config.getPlayerRange().first << ", " << config.getPlayerRange().second << '\n';
        result << "Audience: " << config.hasAudience() << '\n';
        result << "List of Game Setups Below: \n";
        result << " \n"; // this is empty space incase there are multiple set ups and we want to display it nicely.
        // result << "Setup Name: " << setup.name << '\n';
        if (setup.kind.has_value())
        {
            result << "Setup Kind: " << *setup.kind << '\n';
        }
        if (setup.prompt.has_value())
        {
            result << "Setup prompt: " << *setup.prompt << '\n';
        }

        if (setup.getRange().has_value())
        {
            auto range = setup.getRange().value();
            result << "Range: (" << range.first << ", " << range.second << ")" << '\n';
        }

        if (setup.getChoices().has_value())
        {
            auto choices = setup.getChoices().value();
            result << "Choices:" << '\n';
            for (const auto &[key, description] : choices)
            {
                result << "  " << key << ": " << description << '\n';
            }
        }

        if (setup.getDefault().has_value())
        {
            auto defaultValue = setup.getDefault().value();
            result << "Default Value:" << '\n';
            for (const auto &[key, value] : defaultValue)
            {
                result << "  " << key << ": " << value.getType() << '\n';
            }
        }
        result << "Please enter kind and your desired configuration (I.e for range 'integer 1-3', for choice 'enum 1(1 stands for first game starting from top to bottom.):', for true/false 'boolean false', for prompt 'prompt messageyouwanthere') : \n";
        currentGameCreator->editingSetup = true;
        result << " \n"; // this is empty space incase there are multiple set ups and we want to display it nicely.
        ConfigEditResult msg;
        msg.message = result.str();
        msg.status = EditState::Success;
        return msg;
    }
    else if (currentGameCreator->editingSetup)
    {
        auto editResult = parsingEditInput(adminMessage, setup, config);

        ConfigEditResult msg;
        msg.message = result.str();
        if (editResult.editSuccessValue)
        {

            msg.status = EditState::Success;
            currentGameCreator->editingSetup = false; // this is just to indicate that were done editing the current setup
        }
        else
        {
            msg.status = EditState::Error;
        }

        return msg;
    }
    else
    {
        result << "no more setups to edit! \n";

        ConfigEditResult msg;
        msg.message = result.str();
        msg.status = EditState::Done;
        return msg;
    }
    result << "reached end of function in gameConfigEdit \n"; // this is empty space incase there are multiple set ups and we want to display it nicely.

    ConfigEditResult msg;
    msg.message = result.str();
    msg.status = EditState::Success;
    return msg;
}

ParsedEditInput parsingEditInput(const std::string &message, GameConfiguration::Setup &setup, GameConfiguration &config)
{

    ParsedEditInput result;
    std::istringstream stream(message);

    stream >> result.kind; // will extract first word up until it hits whitespace and store into result.kind.
    std::getline(stream, result.value);
    if (result.kind == "integer")
    {

        if (result.value.find('-') == std::string::npos)
        {
            // dealing with just single int
            if (setup.getDefault().has_value())
            {
                auto defaultValue = setup.getDefault().value();
                for (auto &[key, value] : defaultValue)
                {
                    value = result.value;
                    std::cout << "SET DEFAULT VALUE TO NOW BE : " << value << '\n';
                }
            }
        }
        else
        {
            // dealing with range
            char dash = '-';
            std::istringstream range(result.value);
            range >> result.start >> dash >> result.end;
            if (setup.getRange().has_value())
            {
                // auto range = setup.getRange().value();
                // std::pair<int, int> newRange(result.start,result.end);

                // config.setRange("integer",newRange);

                setup.range.value().first = result.start;
                setup.range.value().second = result.end;

                std::cout << "Range SET TO NOW BE: (" << setup.range.value().first << ", " << setup.range.value().second << ")" << '\n';
            }
            return result;
        }
    }
    else if (result.kind == "enum")
    {//enum parsing isn't working in gameConfig at the moment so this will core dump.
        std::cout << "value enum " << result.value << '\n';
        int index = std::stoi(result.value) - 1; // minus 1 to properly find value
        auto &holder = setup.choices.value().at(index);
        setup.chosenChoice = holder.second;

        // setup.chosenChoice = setup.choices.value().at(index).second;
        return result;
    }
    else if (result.kind == "boolean")
    {
    }
    else if (result.kind == "prompt")
    {
    }
    else
    {
        result.editSuccessValue = false;
        return result;
    }
}