#include "PresetLayer.hpp"

using namespace gd;
using namespace gdmake;
using namespace gdmake::extra;
using namespace cocos2d;

void PresetLayer::setup() {
    this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
        .fromNode(CCMenuItemSpriteExtra::create(
            ButtonSprite::create(
                "Create Empty", 0, 0, "goldFont.fnt", "GJ_button_01.png", 0, .8f
            ),
            this,
            (SEL_MenuHandler)&PresetLayer::onCreate
        ))
        .move(0.0f, this->m_pLrSize.height / 2 - 30.0f)
        .done()
    );

    auto ix = 0u;
    for (auto preset : BetterEdit::sharedState()->getPresets()) {
        this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<CCLabelBMFont*>()
                    .fromText(preset.name.c_str(), "goldFont.fnt")
                    .scale(.7f)
                    .done(),
                this,
                (SEL_MenuHandler)&PresetLayer::onCreate
            ))
            .uobj(CCString::create(preset.data))
            .move(0.0f, this->m_pLrSize.height / 2 - 60.0f - ix * 30.0f)
            .done()
        );

        this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"),
                this,
                (SEL_MenuHandler)&PresetLayer::onRemove
            ))
            .udata(ix)
            .move(this->m_pLrSize.width / 2 - 25.0f, this->m_pLrSize.height / 2 - 30.0f - ix * 30.0f)
            .done()
        );

        ix++;
    }
}

void PresetLayer::onCreate(CCObject* pSender) {
    auto lvlStr = as<CCString*>(as<CCNode*>(pSender)->getUserObject());

    auto level = GameLevelManager::createNewLevel();
    if (lvlStr)
        level->setLevelData(lvlStr->getCString());

    EditLevelLayer::scene(level);
}

void PresetLayer::onRemove(CCObject* pSender) {
    BetterEdit::sharedState()->removePreset(
        as<unsigned int>(as<CCNode*>(pSender)->getUserData())
    );
}

PresetLayer* PresetLayer::create() {
    auto pRet = new PresetLayer();

    if (pRet && pRet->init(200.0f, 200.0f)) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
