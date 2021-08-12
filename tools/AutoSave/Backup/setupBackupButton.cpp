#include "setupBackupButton.hpp"
#include "BackupViewLayer.hpp"

class EditLevelLayer_CB : public EditLevelLayer {
    public:
        void onBrowseBackups(CCObject* pSender) {
            BackupViewLayer::scene(this->m_pLevel);
        }
};

void setupBackupButton(EditLevelLayer* layer, GJGameLevel* level) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto btn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_myServerBtn_001.png"),
        layer,
        (SEL_MenuHandler)&EditLevelLayer_CB::onBrowseBackups
    );

    btn->getNormalImage()->setScale(.65f);

    btn->setPosition(-170.0f, 0.0f);

    layer->m_pButtonMenu->addChild(btn);
}
