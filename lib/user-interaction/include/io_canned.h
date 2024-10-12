#ifndef IO_CANNED_H
#define IO_CANNED_H

#include <iostream>
#include <string>
#include <vector>
#include <queue>

#include "io_interface.h"

class IOCanned : public IOInterface
{
public:
    void print(const std::string &message) override
    {
        outputs.push_back(message);
    }

    std::string get_input() override
    {
        if (inputs.empty())
        {
            return "";
        }
        std::string input = inputs.front();
        inputs.pop();
        return input;
    }

    void set_inputs(const std::vector<std::string> &new_inputs)
    {
        for (const auto &input : new_inputs)
        {
            inputs.push(input);
        }
    }

    std::vector<std::string> get_outputs() const
    {
        return outputs;
    }

private:
    std::queue<std::string> inputs;
    std::vector<std::string> outputs;
};

#endif // IO_CANNED_H