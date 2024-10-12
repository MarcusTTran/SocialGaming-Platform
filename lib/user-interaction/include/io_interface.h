#ifndef IO_INTERFACE_H
#define IO_INTERFACE_H

#include <string>

class IOInterface
{
public:
    virtual ~IOInterface() = default;
    virtual void print(const std::string &message) = 0;
    virtual std::string get_input() = 0;
};

#endif // IO_INTERFACE_H