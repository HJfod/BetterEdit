#pragma once

#include "KeybindManager.hpp"

class KeybindNode : public CCSprite {
    protected:
        ButtonSprite* m_pSprite;
        KeybindManager::Target m_obTarget;

        bool init(const char*, bool);
    
    public:
        void updateLabel();

        static KeybindNode* create(const char*, bool addToList = true);

        static CCArray* getList();
        static void clearList();
        static void showList(bool);
};
