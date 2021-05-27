#pragma once

#include <string>

inline int strToInt(const char* str, bool* is = nullptr) {
    bool isStr = true;
    for (auto i = 0u; i < strlen(str); i++)
        if (!isdigit(str[i]) && str[i] != '-')
            isStr = false;
        
    if (is) *is = isStr;

    return isStr ? std::atoi(str) : -1;
}
