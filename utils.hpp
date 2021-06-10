#pragma once

#include <string>
#include <GDMake.h>

inline cocos2d::CCSprite* spriteFNWithScale(const char* name, float scale) {
    auto ret = cocos2d::CCSprite::createWithSpriteFrameName(name);
    if (!ret) return nullptr;

    ret->setScale(scale);

    return ret;
}

inline int strToInt(const char* str, bool* is = nullptr) {
    bool isStr = true;
    for (auto i = 0u; i < strlen(str); i++)
        if (!isdigit(str[i]) && str[i] != '-')
            isStr = false;
        
    if (is) *is = isStr;

    return isStr ? std::atoi(str) : -1;
}
