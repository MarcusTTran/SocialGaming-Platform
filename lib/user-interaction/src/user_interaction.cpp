#include <iostream>

#include "user_interaction.h"

namespace user_interaction
{

    // Function to print a string to the user
    void print_to_user(const std::string &message, IOInterface &io)
    {
        io.print(message);
    }

    // Function to get free-form user input
    std::string get_user_input_free_form(const std::string &prompt, IOInterface &io)
    {
        io.print(prompt);
        return io.get_input();
    }

    // Function to get specific user input from a choice of options
    std::string get_user_input_from_options(const std::string &prompt, const std::vector<std::string> &options, IOInterface &io)
    {
        io.print(prompt);
        for (std::size_t i = 0; i < options.size(); ++i)
        {
            // Is there a way to use an iterator but still print the number?
            io.print(std::to_string(i + 1) + ". " + options[i]);
        }
        io.print("Enter the number of your choice: ");
        while (true)
        {
            std::string input = io.get_input();
            try
            {
                std::size_t pos{};
                std::size_t choice = std::stoi(input, &pos);
                if (pos != input.length())
                {
                    io.print("Only include a whole number.");
                }
                else if (choice <= 0 || choice > options.size())
                {
                    io.print("You must choose between 1 and " + std::to_string(options.size()) + "!");
                }
                else
                {
                    return options[choice - 1];
                }
            }
            catch (const std::invalid_argument &)
            {
                io.print("That's not a number!");
            }
            catch (std::out_of_range &)
            {
                io.print("That's wildly out of scope...");
            }
            io.print("Invalid choice, please try again: ");
        }
    }
}