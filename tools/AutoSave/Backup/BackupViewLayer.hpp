#pragma once

#include "../../../BetterEdit.hpp"

class BackupViewLayer : public CCLayer {
    protected:
        GJGameLevel* m_pLevel;
        CCMenu* m_pButtonMenu;
        GJListLayer* m_pList = nullptr;
        CCLabelBMFont* m_pListLabel;
        std::string m_sTitle;

        bool init(GJGameLevel*);

        void onExit(CCObject*);
        void keyDown(enumKeyCodes) override;
        void onNew(CCObject*);

    public:
        void reloadList();

        static BackupViewLayer* create(GJGameLevel*);
        static BackupViewLayer* scene(GJGameLevel*);
};
