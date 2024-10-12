#ifndef USER_INTERACTION_H
#define USER_INTERACTION_H

#include <string>
#include <vector>

namespace user_interaction
{

    // Function to get free-form user input
    std::string get_user_input_free_form(const std::string &);

    // Function to get specific
    std::string get_user_input_from_options(const std::string &, const std::vector<std::string> &);

}

#endif // USER_INTERACTION_H