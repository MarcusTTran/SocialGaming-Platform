#ifndef USER_INTERACTION_H
#define USER_INTERACTION_H

#include <string>
#include <vector>

#include "io_interface.h"

namespace user_interaction
{

    // Function to get free-form user input
    std::string get_user_input_free_form(const std::string &, IOInterface &);

    // Function to get specific
    std::string get_user_input_from_options(const std::string &, const std::vector<std::string> &, IOInterface &);

}

#endif // USER_INTERACTION_H