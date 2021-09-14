#include "customUI.hpp"
#include "../../hooks/EditorPauseLayer.hpp"

void EditorPauseLayer_CB::onCustomizeUI(CCObject*) {
    UIManager::get()->startCustomizing();
    this->onResume(nullptr);
}

void loadEditorCustomizations(EditorUI*) {

}

void loadUICustomizeBtn(EditorPauseLayer* self) {
    auto menu = as<CCMenu*>(self->m_pButton0->getParent());
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    auto spr = ButtonSprite::create(
        "Customize UI", 0, 0, "goldFont.fnt", "GJ_button_01.png", 0, .8f
    );
    spr->setScale(.7f);
    auto btn = CCMenuItemSpriteExtra::create(
        spr, self, menu_selector(EditorPauseLayer_CB::onCustomizeUI)
    );
    btn->setPosition(0.0f, winSize.height - 60.0f);
    menu->addChild(btn);
}

