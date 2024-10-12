#include "user_interaction.h"
#include <iostream>

namespace user_interaction
{
    // Function to get free-form user input
    std::string get_user_input_free_form(const std::string &prompt, IOInterface &io)
    {
        io.print(prompt);
        return io.get_input();
    }

    // Function to get specific user input from options
    std::string get_user_input_from_options(const std::string &prompt, const std::vector<std::string> &options, IOInterface &io)
    {
        io.print(prompt);
        for (std::size_t i = 0; i < options.size(); ++i)
        {
            // Is there a way to use an iterator but still print the number?
            io.print(std::to_string(i + 1) + ". " + options[i] + "\n");
        }
        io.print("Enter the number of your choice: ");
        while (true)
        {
            std::string input = io.get_input();
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
            catch (std::out_of_range &)
            {
                // too big, loop
            }
            io.print("Invalid choice, please try again: ");
        }
    }
}