#ifndef IO_COMMANDLINE_H
#define IO_COMMANDLINE_H

#include <iostream>
#include <string>

#include "io_interface.h"

class IOCommandline : public IOInterface
{
public:
    // should these implementations go in a .cpp file?
    void print(const std::string &message) override
    {
        std::cout << message << std::endl;
    }

    std::string get_input() override
    {
        std::string input;
        std::getline(std::cin, input);
        return input;
    }
};

#endif // IO_COMMANDLINE_H