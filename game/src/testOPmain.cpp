#include <iostream>
#include <memory>
#include <unordered_map>
#include <string>
#include "optionsDisplay.h"
int main()
{
    try
    {
        // Test ConfigurationOptions
        std::unordered_map<std::string, std::string> enumChoices = {
            {"fast", "A quick round with friends"},
            {"standard", "Standard play"},
            {"long", "A marathon battle against former friends"}};
        namePlayerAudience test = {"Rock Paper Scissors", std::make_pair(1, 10), true};

        auto enumConfig = createConfigurationOptions(test, "enum", "Game Style", &enumChoices, 0, 0);
        std::cout << enumConfig->displayOptions() << '\n';

        auto boolConfig = createConfigurationOptions(test, "boolean", "Color is blue", nullptr, 0, 0);
        std::cout << boolConfig->displayOptions() << '\n';

        auto intConfig = createConfigurationOptions(test, "integer", "Number of players", nullptr, 1, 10);
        std::cout << intConfig->displayOptions() << '\n';
        // Test Invalid
        auto invalidConfig = createConfigurationOptions(test, "invalid", "Number of players", nullptr, 1, 10);
        std::cout << intConfig->displayOptions() << '\n';
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << '\n';
    }

    return 0;
}
