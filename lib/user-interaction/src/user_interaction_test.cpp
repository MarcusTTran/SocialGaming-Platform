#include <iostream>

#include "io_commandline.h"
#include "user_interaction.h"

int main()
{
    IOCommandline io;
    std::string free_form_input = user_interaction::get_user_input_free_form("Enter something: ", io);
    std::cout << "You entered: " << free_form_input << std::endl;

    std::vector<std::string> options = {"Option 1", "Option 2", "Option 3"};
    std::string choice = user_interaction::get_user_input_from_options("Choose an option:", options, io);
    std::cout << "You chose: " << choice << std::endl;

    return 0;
}