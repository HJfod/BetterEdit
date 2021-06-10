#pragma once

#include <string>
#include <GDMake.h>

class CCSpriteConstructor : public cocos2d::CCSprite {
    public:
        inline static CCSpriteConstructor* fromFrameName(const char* fname) {
            return reinterpret_cast<CCSpriteConstructor*>(
                createWithSpriteFrameName(fname)
            );
        }
        inline CCSpriteConstructor* flipX(bool flip = true) {
            this->CCSprite::setFlipX(flip);
            return this;
        }
        inline CCSpriteConstructor* flipY(bool flip = true) {
            this->CCSprite::setFlipY(flip);
            return this;
        }
        inline CCSpriteConstructor* rotate(float rot) {
            this->CCSprite::setRotation(rot);
            return this;
        }
        inline CCSpriteConstructor* scale(float scale) {
            this->CCSprite::setScale(scale);
            return this;
        }
};

inline int strToInt(const char* str, bool* is = nullptr) {
    bool isStr = true;
    for (auto i = 0u; i < strlen(str); i++)
        if (!isdigit(str[i]) && str[i] != '-')
            isStr = false;
        
    if (is) *is = isStr;

    return isStr ? std::atoi(str) : -1;
}
