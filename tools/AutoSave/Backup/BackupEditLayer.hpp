#pragma once

#include "LevelBackupManager.hpp"
#include "BackupViewLayer.hpp"
#include <BrownAlertDelegate.hpp>
#include <InputNode.hpp>

class BackupEditLayer : public BrownAlertDelegate, TextInputDelegate {
    protected:
        BackupViewLayer* m_pBackupLayer;
        LevelBackup* m_pBackup;
        InputNode* m_pNameInput;

        void setup() override;

        void textChanged(CCTextInputNode*) override;

    public:
        static BackupEditLayer* create(BackupViewLayer*, LevelBackup*);
};

