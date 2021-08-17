#pragma once

#include "LevelBackupManager.hpp"
#include "BackupViewLayer.hpp"
#include "BackupListView.hpp"
#include <BrownAlertDelegate.hpp>
#include <InputNode.hpp>

class BackupViewLayer;
class BackupCell;

class BackupEditLayer :
    public BrownAlertDelegate,
    TextInputDelegate,
    FLAlertLayerProtocol
{
    protected:
        BackupViewLayer* m_pBackupLayer;
        LevelBackup* m_pBackup;
        BackupCell* m_pCell;
        InputNode* m_pNameInput;

        void setup() override;

        void textChanged(CCTextInputNode*) override;
        void applyBackup();

        void onResetName(CCObject*);
        void onDelete(CCObject*);
        void onView(CCObject*);
        void onApply(CCObject*);

        void FLAlert_Clicked(FLAlertLayer*, bool) override;

        CCSprite* createBtnSprite(const char* spr, const char* text, ccColor3B color = { 255, 255, 255 });

    public:
        static BackupEditLayer* create(BackupViewLayer*, BackupCell*, LevelBackup*);
};

