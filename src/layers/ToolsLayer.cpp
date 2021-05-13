#include "ToolsLayer.hpp"
#include "../tools/RemapLayer.hpp"
#include "../tools/PasteLayer.hpp"

void ToolsLayer::onRemapIDs(cocos2d::CCObject*) {
    this->onClose(nullptr);

    RemapLayer::create()->show();
}

void ToolsLayer::onPasteString(cocos2d::CCObject*) {
    this->onClose(nullptr);

    PasteLayer::create()->show();
}

void ToolsLayer::setup() {
    for (auto m : std::vector<std::tuple<const char*, void(ToolsLayer::*)(cocos2d::CCObject*)>> {
        { "Remap IDs", &ToolsLayer::onRemapIDs },
        { "Paste from Object String", &ToolsLayer::onPasteString },
        { "More Options", nullptr },
        { "n shit", nullptr }
    })
        this->m_pButtonMenu->addChild(
            gd::CCMenuItemSpriteExtra::create(
                gd::ButtonSprite::create(
                    std::get<0>(m), (int)this->m_pLrSize.width - 100, true, "goldFont.fnt", "GJ_Button_01.png", 0, .8f
                ),
                this,
                (cocos2d::SEL_MenuHandler)(std::get<1>(m))
            )
        );

    this->m_pButtonMenu->alignItemsVerticallyWithPadding(5.0f);
}

ToolsLayer* ToolsLayer::create() {
    auto pRet = new ToolsLayer();

    if (pRet && pRet->init(320.0f, 260.0f, "GJ_square01.png", "BetterEdit Tools")) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}

