#include "TemplateMenu.hpp"

using namespace cocos2d;
using namespace cocos2d::extension;
using namespace gd;

CCSprite* makeEditSideBtn(const char* sprName) {
    auto bg = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
    bg->setOpacity(100);

    auto aspr = CCSprite::createWithSpriteFrameName(sprName);

    aspr->setPosition(bg->getContentSize() / 2);
    aspr->setScale(.825f);

    bg->addChild(aspr);

    bg->setScale(.9f);

    return bg;
}

CCSprite* makeEditBtn(const char* sprName) {
    auto bg = CCSprite::create("GJ_button_05.png");

    auto aspr = CCSprite::createWithSpriteFrameName(sprName);

    aspr->setPosition(bg->getContentSize() / 2);
    aspr->setScale(.825f);

    bg->addChild(aspr);

    bg->setScale(.6f);

    return bg;
}

bool TemplateMenu::init(CCArray* buttons, int idk, bool idkb, int rowCount, int columnCount, CCPoint pos) {
    if (!gd::EditButtonBar::init(buttons, idk, idkb, rowCount, columnCount, pos))
        return false;
    
    this->m_nMode = TMode::kTModeNormal;
    this->m_obSize = CCSize { 200.0f, 60.0f };
    
    this->m_pEditMenu = CCMenu::create();
    this->m_pEditMenu->setPosition(this->m_obPosition);

    this->setupEditMenu();

    this->addChild(this->m_pEditMenu);

    this->m_pEditMenu->setVisible(false);
    
    return true;
}

void TemplateMenu::onSaveEdit(CCObject* pSender) {
    this->switchMode(TMode::kTModeNormal);
}

void TemplateMenu::onCancelEdit(CCObject* pSender) {
    this->switchMode(TMode::kTModeNormal);
}

void TemplateMenu::setupEditMenu() {
    auto label = CCLabelBMFont::create("Create Template", "goldFont.fnt");
    label->setPosition(0, -10.0f);
    label->setScale(.55f);
    this->m_pEditMenu->addChild(label);

    auto saveBtn = CCMenuItemSpriteExtra::create(
        makeEditSideBtn("GJ_completesIcon_001.png"),
        this,
        (SEL_MenuHandler)&TemplateMenu::onSaveEdit
    );
    saveBtn->setPosition(150.0f, -60.0f);
    this->m_pEditMenu->addChild(saveBtn);

    auto cancelBtn = CCMenuItemSpriteExtra::create(
        makeEditSideBtn("GJ_deleteIcon_001.png"),
        this,
        (SEL_MenuHandler)&TemplateMenu::onCancelEdit
    );
    cancelBtn->setPosition(150.0f, -20.0f);
    this->m_pEditMenu->addChild(cancelBtn);

    using uint = unsigned int;

    uint rowmax = 4u;
    uint ix = 0u;
    constexpr const float spacing = 25.0f;
    constexpr const float offset = 0.0f;

    float xpos = -spacing * (rowmax / 2u) - offset;
    float ypos = -35.0f;

    for (auto spr : std::vector<const char*> {
        "blockOutline_01_001.png",
        "blockOutline_02_001.png",
        "blockOutline_03_001.png",
        "blockOutline_04_001.png",
        "blockOutline_05_001.png",
        "blockOutline_06_001.png",
        "blockOutline_14_001.png",
        "blockOutline_15_001.png",
    }) {
        auto btn = CCMenuItemSpriteExtra::create(
            makeEditBtn(spr),
            this,
            nullptr
        );

        btn->setPosition(xpos, ypos);

        ix++;

        xpos += spacing;

        if (ix % rowmax == 0) {
            xpos = -spacing * (rowmax / 2u) - offset;
            ypos -= spacing;
        }

        this->m_pEditMenu->addChild(btn);
    }
}

void TemplateMenu::switchMode(TemplateMenu::TMode mode) {
    this->m_nMode = mode;

    this->m_pScrollLayer->setVisible(mode == TMode::kTModeNormal);
    this->m_pEditMenu->setVisible(mode == TMode::kTModeEdit);
}

TemplateMenu* TemplateMenu::create(CCArray* buttons, CCPoint pos, int idk, bool idkb, int rowCount, int columnCount) {
    auto ret = new TemplateMenu();

    if (ret && ret->init(buttons, idk, idkb, rowCount, columnCount, pos)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
