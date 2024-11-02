#include <iostream>
#include <map>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

#include "optionsDisplay.h"

std::unique_ptr<ConfigurationOptions> createConfigurationOptions(namePlayerAudience npa, const std::string kind, const std::string prompt,
                                                                 std::unordered_map<std::string, std::string> *choices,
                                                                 int minValue, int maxValue)
{

    if (kind == "enum")
    {
        return std::make_unique<EnumConfigurationOptions>(npa, prompt, *choices);
    }
    else if (kind == "boolean")
    {
        return std::make_unique<BooleanConfigurationOptions>(npa, prompt);
    }
    else if (kind == "integer")
    {
        return std::make_unique<IntegerConfigurationOptions>(npa, prompt, minValue, maxValue, 0);
    }

    throw std::invalid_argument("Unsupported kind");
}
