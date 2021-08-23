#include "BackupScheduleLayer.hpp"

void BackupScheduleLayer::setup() {
    m_pBackup = LevelBackupManager::get()->getBackupSettingsForLevel(this->m_pLevel, true);
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    if (!m_pBackup) return;

    GameToolbox::createToggleButton(
        menu_selector(BackupScheduleLayer::onToggleAutoBackup),
        m_pBackup->m_bAutoBackup, this->m_pButtonMenu,
        this, this->m_pLayer, .7f, .7f, 160.0f,
        "", false, 0, nullptr,
        "Enable Auto-Backups",
        winSize / 2 + CCPoint { -85.0f, 40.0f },
        { 5.0f, 0.0f }
    );

    m_pLabel1 = CCLabelBMFont::create("Backup After", "bigFont.fnt");
    m_pLabel1->setPosition(winSize / 2 + CCPoint { 0.0f, 10.0f });
    m_pLabel1->setScale(.5f);
    this->m_pLayer->addChild(m_pLabel1);

    m_pInput = InputNode::create(100.f, "x");
    // m_pInput->setString(std::to_string(m_pBackup->m_nBackupEvery).c_str());
    m_pInput->setPosition(winSize / 2 + CCPoint { 0.0f, -20.0f });
    m_pInput->getInputNode()->setDelegate(this);
    this->m_pLayer->addChild(m_pInput);

    m_pLabel2 = CCLabelBMFont::create("Objects Placed", "bigFont.fnt");
    m_pLabel2->setPosition(winSize / 2 + CCPoint { 0.0f, -50.0f });
    m_pLabel2->setScale(.5f);
    this->m_pLayer->addChild(m_pLabel2);

    // auto label3 = CCLabelBMFont::create(
    //     (
    //         "Objects added since last backup: " +
    //         std::to_string(m_pLevel->objectCount - m_pBackup->m_nLastBackupObjectCount)
    //     ).c_str(), "goldFont.fnt"
    // );
    // label3->setPosition(winSize / 2 + CCPoint { 0.0f, -70.0f });
    // label3->setScale(.3f);
    // this->m_pLayer->addChild(label3);

    this->m_pButtonMenu->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<ButtonSprite*>()
                    .fromNode(ButtonSprite::create(
                        "OK", 0, 0, "goldFont.fnt", "GJ_button_01.png", 0, .8f
                    ))
                    .scale(.8f)
                    .done(),
                this,
                (SEL_MenuHandler)&BackupScheduleLayer::onClose
            ))
            .move(0.0f, - this->m_pLrSize.height / 2 + 25.0f)
            .done()
    );

    this->onToggleAutoBackup(nullptr);
}

void BackupScheduleLayer::textChanged(CCTextInputNode* input) {
    if (input && input->getString() && strlen(input->getString())) {
        auto num = strToInt(input->getString());

        if (num < 1)
            num = LevelBackupSettings::s_defaultBackupEvery;
        
        m_pBackup->m_nBackupEvery = num;
    }
}

void BackupScheduleLayer::onToggleAutoBackup(CCObject* pSender) {
    if (pSender)
        m_pBackup->m_bAutoBackup = !as<CCMenuItemToggler*>(pSender)->isToggled();
    
    auto b = m_pBackup->m_bAutoBackup;

    if (m_pLabel1) {
        m_pLabel1->setOpacity(b ? 255 : 100);
        m_pLabel1->setColor(b ? cc3x(0xfff) : cc3x(0x777));

        m_pLabel2->setOpacity(b ? 255 : 100);
        m_pLabel2->setColor(b ? cc3x(0xfff) : cc3x(0x777));

        // m_pInput->setEnabled(b);
        m_pInput->getInputNode()->m_pTextField->setOpacity(b ? 255 : 100);
    }
}

BackupScheduleLayer* BackupScheduleLayer::create(GJGameLevel* level) {
    auto ret = new BackupScheduleLayer;

    if (
        ret &&
        (ret->m_pLevel = level) &&
        ret->init(280.0f, 230.0f, "GJ_square01.png", "Schedule Backups")
    ) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
