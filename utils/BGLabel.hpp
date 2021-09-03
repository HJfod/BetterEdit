#pragma once

#include "../BetterEdit.hpp"

class BGLabel : public CCNode {
    protected:
        CCScale9Sprite* m_pBGSprite;
        CCLabelBMFont* m_pLabel;

        bool init(const char* text, const char* font);
    
    public:
        void setString(const char* text);
        void updateSize();

        static BGLabel* create(const char* text, const char* font);
};
