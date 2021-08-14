#include "BackupEditLayer.hpp"

void BackupEditLayer::setup() {

}

void BackupEditLayer::textChanged(CCTextInputNode* input) {
    
}

BackupEditLayer* BackupEditLayer::create(BackupViewLayer* layer, LevelBackup* backup) {
    auto pRet = new BackupEditLayer;

    if (
        pRet &&
        (pRet->m_pBackup = backup) &&
        (pRet->m_pBackupLayer = layer) &&
        pRet->init(300.0f, 260.0f)
    ) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
