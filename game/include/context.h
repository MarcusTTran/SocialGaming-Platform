#pragma once

#include <string>

/*
 * Name resolution
 */

template <typename T>
struct Variable
{
    std::string name;
    T *value;
};
