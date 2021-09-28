#include "LevelBackupManager.hpp"

GDMAKE_HOOK(0x5be30, "_ZN9LevelCell18loadLocalLevelCellEv")
void __fastcall LevelCell_loadLocalLevelCell(LevelCell* self) {
    GDMAKE_ORIG_V(self);

    if (LevelBackupManager::get()->levelHasBackups(self->m_pLevel)) {
        auto label = getChild<CCLabelBMFont*>(self->m_pLayer, 0);

        auto backupIcon = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");

        auto bLabel = CCLabelBMFont::create("B", "bigFont.fnt");

        bLabel->setScale(.9f);
        // bLabel->setColor({ 0, 0, 0 });
        bLabel->setPosition(backupIcon->getContentSize() / 2);

        backupIcon->addChild(bLabel);

        backupIcon->setScale(.35f);
        backupIcon->setColor({ 55, 185, 255 });
        backupIcon->setPosition({
            label->getPositionX() + label->getScaledContentSize().width + 12.5f,
            label->getPositionY()
        });

        self->m_pLayer->addChild(backupIcon);
    }
}
