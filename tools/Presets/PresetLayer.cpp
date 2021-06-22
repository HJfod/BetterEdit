#include "PresetLayer.hpp"
#include <InputPrompt.hpp>

using namespace gd;
using namespace gdmake;
using namespace gdmake::extra;
using namespace cocos2d;

void PresetLayer::setup() {
    this->m_pSelectedLabel = CCLabelBMFont::create("Empty", "bigFont.fnt");
    this->m_pSelectedLabel->setPosition(0, 0);
    this->m_pSelectedLabel->limitLabelWidth(this->m_pLrSize.width - 90.0f, 1.0f, .2f);
    this->m_pButtonMenu->addChild(this->m_pSelectedLabel);

    this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
        .fromNode(CCMenuItemSpriteExtra::create(
            CCNodeConstructor()
                .fromFrameName("GJ_arrow_01_001.png")
                .scale(.8f)
                .done(),
            this,
            (SEL_MenuHandler)&PresetLayer::onPage
        ))
        .udata(-1)
        .move(- this->m_pLrSize.width / 2 + 25.0f, 0.0f)
        .done()
    );
    this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
        .fromNode(CCMenuItemSpriteExtra::create(
            CCNodeConstructor()
                .fromFrameName("GJ_arrow_01_001.png")
                .scale(.8f)
                .flipX()
                .done(),
            this,
            (SEL_MenuHandler)&PresetLayer::onPage
        ))
        .udata(1)
        .move(this->m_pLrSize.width / 2 - 25.0f, 0.0f)
        .done()
    );

    this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
        .fromNode(CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"),
            this,
            (SEL_MenuHandler)&PresetLayer::onRemove
        ))
        .save(&m_pDeleteButton)
        .exec([](auto t) -> void { t->setVisible(false); })
        .move(70.0f, - this->m_pLrSize.height / 2 + 30.0f)
        .done()
    );

    this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
        .fromNode(CCMenuItemSpriteExtra::create(
            CCNodeConstructor()
                .fromFrameName("GJ_optionsBtn_001.png")
                .scale(.7f)
                .done(),
            this,
            (SEL_MenuHandler)&PresetLayer::onRename
        ))
        .save(&m_pSettingsButton)
        .exec([](auto t) -> void { t->setVisible(false); })
        .move(-70.0f, - this->m_pLrSize.height / 2 + 30.0f)
        .done()
    );

    this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
        .fromNode(CCMenuItemSpriteExtra::create(
            ButtonSprite::create(
                "Create", 0, 0, "goldFont.fnt", "GJ_button_01.png", 0, .8f
            ),
            this,
            (SEL_MenuHandler)&PresetLayer::onCreate
        ))
        .move(0.0f, - this->m_pLrSize.height / 2 + 30.0f)
        .done()
    );
}

void PresetLayer::updateLabel(const char* text) {
    this->m_pSelectedLabel->setString(text);
    this->m_pSelectedLabel->limitLabelWidth(this->m_pLrSize.width - 90.0f, 1.0f, .2f);
}

void PresetLayer::onPage(CCObject* pSender) {
    this->m_nSelectedIndex += as<int>(as<CCNode*>(pSender)->getUserData());

    if (this->m_nSelectedIndex < 0)
        this->m_nSelectedIndex = BetterEdit::sharedState()->getPresets().size();

    if (this->m_nSelectedIndex > (int)BetterEdit::sharedState()->getPresets().size())
        this->m_nSelectedIndex = 0;

    std::string text;
    if (this->m_nSelectedIndex)
        text = BetterEdit::sharedState()->getPresets().at(this->m_nSelectedIndex - 1).name;
    else
        text = "Empty";
    
    this->updateLabel(text.c_str());

    this->m_pDeleteButton->setVisible(this->m_nSelectedIndex);
    this->m_pSettingsButton->setVisible(this->m_nSelectedIndex);
}

void PresetLayer::onCreate(CCObject* pSender) {
    std::string lvlStr;
    if (this->m_nSelectedIndex)
        lvlStr = BetterEdit::sharedState()->getPresets().at(this->m_nSelectedIndex - 1).data;
    else
        lvlStr = "";

    auto level = GameLevelManager::createNewLevel();
    if (lvlStr.size())
        level->setLevelData(lvlStr.c_str());

    EditLevelLayer::scene(level);
}

void PresetLayer::onRemove(CCObject* pSender) {
    if (this->m_nSelectedIndex)
        BetterEdit::sharedState()->removePreset(
            this->m_nSelectedIndex - 1
        );
    
    this->onClose(nullptr);

    PresetLayer::create()->show();
}

void PresetLayer::onRename(CCObject* pSender) {
    if (this->m_nSelectedIndex)
        InputPrompt::create("Rename Preset", "Name", [this](const char* val) -> void {
            if (!this->m_nSelectedIndex) return;

            if (val && strlen(val))
                BetterEdit::sharedState()->getPresets()[this->m_nSelectedIndex - 1].name = val;
            
            this->updateLabel(BetterEdit::sharedState()->getPresets()[this->m_nSelectedIndex - 1].name.c_str());
        })->show();
}

PresetLayer* PresetLayer::create() {
    auto pRet = new PresetLayer();

    if (pRet && pRet->init(280.0f, 160.0f, "GJ_square01.png", "Select Preset")) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
