#pragma once

#include "LevelBackupManager.hpp"
#include <BrownAlertDelegate.hpp>
#include <InputNode.hpp>

class BackupScheduleLayer : public BrownAlertDelegate, TextInputDelegate {
    protected:
        GJGameLevel* m_pLevel;
        LevelBackupSettings* m_pBackup;
        InputNode* m_pInput;
        CCLabelBMFont* m_pLabel1;
        CCLabelBMFont* m_pLabel2;

        void setup() override;
        void onToggleAutoBackup(CCObject*);
        void textChanged(CCTextInputNode*) override;

    public:
        static BackupScheduleLayer* create(GJGameLevel* level);
};
