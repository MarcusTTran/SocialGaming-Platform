
#include "GameConfigEdit.h"
#include <iostream>
#include <string>

ConfigEditResult editingGameConfig(const GameConfiguration &config, const GameCreators *currentGameCreator)
{
    std::ostringstream result;
    result << "Name: " << config.getGameName().getName() << '\n';
    result << "Player Range: " << config.getPlayerRange().first << ", " << config.getPlayerRange().second << '\n';
    result << "Audience: " << config.hasAudience() << '\n';
    result << "List of Game Setups Below: \n";
    result << " \n"; // this is empty space incase there are multiple set ups and we want to display it nicely.
    auto setups = config.getSetup();
    if (currentGameCreator->gameConfigIterator < currentGameCreator->howManyGamesAdminHasToSet)
    {
        auto &setup = setups.at(currentGameCreator->gameConfigIterator);
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
    }
    else
    {
        result << "no more setups to edit! \n";

        ConfigEditResult msg;
        msg.message = result.str();
        msg.status = EditState::Success;
        return msg;
    }
    result << " \n"; // this is empty space incase there are multiple set ups and we want to display it nicely.

    ConfigEditResult msg;
    msg.message = result.str();
    msg.status = EditState::Success;
    return msg;
}