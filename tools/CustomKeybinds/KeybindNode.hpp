#pragma once

#include "KeybindManager.hpp"

bool isNodeVisible(CCNode* t);

class KeybindNode : public CCSprite {
    protected:
        CCNode* m_pTarget;
        ButtonSprite* m_pSprite;
        KeybindManager::Target m_obTarget;

        bool init(CCNode*, const char*, bool);
    
    public:
        void updateLabel();
        CCNode* getTarget();

        static KeybindNode* create(CCNode*, const char*, bool addToList = true);

        static CCArray* getList();
        static void clearList();
        static void showList(bool);
        static void updateList();
};
