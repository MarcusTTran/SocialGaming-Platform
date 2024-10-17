#include <iostream>
#include <assert.h> //TODO: replace this with google test

#include "io_commandline.h"
#include "io_canned.h"
#include "user_interaction.h"

int main()
{
    // Automated test
    // arrange
    IOCanned io_canned;
    std::string test_input = "test input";
    std::string test_output = "Enter something: ";
    io_canned.set_inputs({test_input});
    // act
    std::string result = user_interaction::get_user_input_free_form(test_output, io_canned);
    // assert
    assert(result == test_input);
    assert(io_canned.get_outputs()[0] == test_output);
    std::cout << "Automated tests passed!" << std::endl;

    // Interactive test
    IOCommandline io;

    user_interaction::print_to_user("Welcome to some example prompts!", io);

    std::string free_form_input = user_interaction::get_user_input_free_form("Enter something: ", io);
    std::cout << "You entered: " << free_form_input << std::endl;

    std::vector<std::string> options = {"Option 1", "Option 2", "Option 3"};
    std::string choice = user_interaction::get_user_input_from_options("Choose an option:", options, io);
    std::cout << "You chose: " << choice << std::endl;

    return 0;
}