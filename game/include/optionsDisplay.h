#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include "GameConfiguration.h"

/* configuration {
  name: "Rock, Paper, Scissors"
  player range: (2, 4)
  audience: false
  setup: {
    rounds {
      kind: integer
      prompt: "The number of rounds to play"
      range: (1, 20)
    }
  }
} */
// display ConfigurationOptions options
struct namePlayerAudience
{
    std::string name;
    std::pair<int, int> playerRange;
    bool audience;
};
class ConfigurationOptions
{
public:
    virtual ~ConfigurationOptions() = default;
    virtual std::string displayOptions() const = 0; // derived classes will return a string obj so it can be used to send messages to queue.
};

// ConfigurationOptions kinds can be 'boolean', 'integer', 'string', 'enum', 'question-answer', 'multiple-choice', 'json'.

class EnumConfigurationOptions : public ConfigurationOptions
{
public:
    EnumConfigurationOptions(const GameConfiguration::Setup& object) : object(object) {}

    std::string displayOptions() const
    {
        std::string holder = (object.prompt) ? object.prompt.value() : "empty prompt.";
        std::ostringstream output;
        output << "name: " << object.name << '\n';
        output << "player range: " << " (" << object.getRange().value().first << ", " << object.getRange().value().second << ")\n";
        std::string val = (true) ? "true" : "false";//temporary, have to pass in bool value from gameconfig object
        output << "audience: " << val << '\n';
        output << holder << ":\n";
        for (const auto &[key, description] : object.choices.value())
        {
            output << " - " << key << ": " << description << "\n";
        }
        return output.str();
    }

private:
    std::string prompt;
    GameConfiguration::Setup object;
    namePlayerAudience npa;
    std::unordered_map<std::string, std::string> choices; // option -> description
};

class BooleanConfigurationOptions : public ConfigurationOptions
{

public:
    BooleanConfigurationOptions(namePlayerAudience npa, const std::string &prompt) : prompt(prompt), npa(npa) {}

    std::string displayOptions() const
    {
        std::ostringstream output;
        output << "name: " << npa.name << '\n';
        output << "player range: " << " (" << npa.playerRange.first << ", " << npa.playerRange.second << ")\n";
        std::string val = (npa.audience) ? "true" : "false";
        output << "audience: " << val << '\n';
        output << prompt << ": (true/false)\n";

        return output.str();
    }

private:
    namePlayerAudience npa;
    std::string prompt;
};

class IntegerConfigurationOptions : public ConfigurationOptions
{

public:
    IntegerConfigurationOptions(const GameConfiguration::Setup& object) : object(object) {}

    std::string displayOptions() const
    {
        std::ostringstream output;
        output << "name: " << object.name << '\n';
        output << "player range: " << " (" << object.getRange().value().first << ", " << object.getRange().value().second << ")\n";
        std::string val = (true) ? "true" : "false";
        output << "audience: " << val << '\n';
        output << prompt << " (Range: " << object.getRange().value().first << " to " <<  object.getRange().value().second << ")\n"; //temprorary, have to pass in main game obj values?
        output << "default: " << 0 << "\n";

        return output.str();
    }

private:
    std::string prompt;
    GameConfiguration::Setup object;
    namePlayerAudience npa;
    int minValue;
    int maxValue;
    int defaultValue;
};

std::unique_ptr<ConfigurationOptions> createConfigurationOptions(const GameConfiguration* object);
