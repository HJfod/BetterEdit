#pragma once

#include "../offsets.hpp"

class InputNode : public cocos2d::CCNode {
    protected:
        cocos2d::extension::CCScale9Sprite* m_pBG;
        gd::CCTextInputNode* m_pInput;
        bool init(float, float, const char*, const char*, const std::string &, int);
        bool init(float, const char*, const char*, const std::string &, int);
        bool initMulti(float, float, const char*, const char*, const std::string &, int);

    public:
        static InputNode* create(float, const char*, const char*, const std::string &, int);
        static InputNode* create(float, const char*, const std::string &, int);
        static InputNode* create(float, const char*, const std::string &);
        static InputNode* create(float, const char*, const char*);
        static InputNode* create(float, const char*);

        static InputNode* createMulti(float, float, const char*, const char*, const std::string &, int);
        static InputNode* createMulti(float, float, const char*, const char*, int);

        void setString(const char*);
        const char* getString();
};