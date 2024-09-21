// src/utils.cpp
#include "utils.h"
#include <stdexcept>
#include <limits>
#include <cstdlib>

int safeStoi(const char* str, const std::string& paramName) {
    char* end;
    long value = std::strtol(str, &end, 10);
    
    if (end == str || *end != '\0') 
        throw std::invalid_argument("Invalid " + paramName + ": '" + str + "' is not a valid integer.");

    if (value < 0 || value > std::numeric_limits<int>::max())
        throw std::out_of_range(paramName + " out of range: " + str);

    return static_cast<int>(value);
}