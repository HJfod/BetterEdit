#pragma once

#include "LevelBackupManager.hpp"
#include "BackupViewLayer.hpp"
#include "BackupListView.hpp"
#include <BrownAlertDelegate.hpp>
#include <InputNode.hpp>

class BackupViewLayer;
class BackupCell;

class BackupEditLayer : public BrownAlertDelegate, TextInputDelegate {
    protected:
        BackupViewLayer* m_pBackupLayer;
        LevelBackup* m_pBackup;
        BackupCell* m_pCell;
        InputNode* m_pNameInput;

        void setup() override;

        void textChanged(CCTextInputNode*) override;

        void onResetName(CCObject*);
        void onDelete(CCObject*);
        void onView(CCObject*);
        void onApply(CCObject*);

        CCSprite* createBtnSprite(const char* spr, const char* text, ccColor3B color);

    public:
        static BackupEditLayer* create(BackupViewLayer*, BackupCell*, LevelBackup*);
};

