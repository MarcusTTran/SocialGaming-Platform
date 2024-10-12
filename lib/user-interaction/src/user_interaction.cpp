#include "user_interaction.h"
#include <iostream>

namespace user_interaction
{
    // Function to get free-form user input
    std::string get_user_input_free_form(const std::string &prompt)
    {
        std::string input;
        std::cout << prompt;
        std::getline(std::cin, input);
        return input;
    }

    // Function to get specific user input from options
    std::string get_user_input_from_options(const std::string &prompt, const std::vector<std::string> &options)
    {
        std::string input;
        std::cout << prompt << std::endl;
        for (std::size_t i = 0; i < options.size(); ++i)
        {
            // Is there a way to use an iterator but still print the number?
            std::cout << i + 1 << ". " << options[i] << std::endl;
        }
        std::cout << "Enter the number of your choice: ";
        while (true)
        {
            std::getline(std::cin, input);
            try
            {
                std::size_t choice = std::stoi(input);
                if (choice > 0 && choice <= options.size())
                {
                    return options[choice - 1];
                }
            }
            catch (const std::invalid_argument &)
            {
                // invalid, loop
            }
            std::cout << "Invalid choice, please try again." << std::endl;
        }
    }
}