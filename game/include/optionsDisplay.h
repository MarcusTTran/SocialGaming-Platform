#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

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
    EnumConfigurationOptions(namePlayerAudience npa, const std::string &prompt, const std::unordered_map<std::string, std::string> &choices)
        : prompt(prompt), choices(choices), npa(npa) {}

    std::string displayOptions() const
    {
        std::ostringstream output;
        output << "name: " << npa.name << '\n';
        output << "player range: " << " (" << npa.playerRange.first << ", " << npa.playerRange.second << ")\n";
        std::string val = (npa.audience) ? "true" : "false";
        output << "audience: " << val << '\n';
        output << prompt << ":\n";
        for (const auto &choice : choices)
        {
            output << " - " << choice.first << ": " << choice.second << "\n";
        }
        return output.str();
    }

private:
    std::string prompt;
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
    IntegerConfigurationOptions(namePlayerAudience npa, const std::string &prompt, int minValue, int maxValue, int defaultValue)
        : prompt(prompt), minValue(minValue), maxValue(maxValue),
          defaultValue(defaultValue), npa(npa) {}

    std::string displayOptions() const
    {
        std::ostringstream output;
        output << "name: " << npa.name << '\n';
        output << "player range: " << " (" << npa.playerRange.first << ", " << npa.playerRange.second << ")\n";
        std::string val = (npa.audience) ? "true" : "false";
        output << "audience: " << val << '\n';
        output << prompt << " (Range: " << minValue << " to " << maxValue << ")\n";
        output << "default: " << defaultValue << "\n";

        return output.str();
    }

private:
    std::string prompt;
    namePlayerAudience npa;
    int minValue;
    int maxValue;
    int defaultValue;
};

std::unique_ptr<ConfigurationOptions> createConfigurationOptions(namePlayerAudience npa, const std::string kind, const std::string prompt,
                                                                 std::unordered_map<std::string, std::string> *choices,
                                                                 int minValue, int maxValue);
