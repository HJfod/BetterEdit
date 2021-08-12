#pragma once

#include "../../../BetterEdit.hpp"

class BackupViewLayer : public CCLayer {
    protected:
        GJGameLevel* m_pLevel;
        CCMenu* m_pButtonMenu;
        GJListLayer* m_pList;

        bool init(GJGameLevel*);

        void onExit(CCObject*);
        void keyDown(enumKeyCodes) override;

    public:
        static BackupViewLayer* create(GJGameLevel*);
        static BackupViewLayer* scene(GJGameLevel*);
};
