#include "customUI.hpp"
#include "../../hooks/EditorPauseLayer.hpp"

void EditorPauseLayer_CB::onCustomizeUI(CCObject*) {
    UIManager::get()->startCustomizing();
    this->onResume(nullptr);
}

void loadEditorCustomizations(EditorUI* self) {
    self->m_pCopyBtn->removeFromParent();
    self->m_pPasteBtn->removeFromParent();
    self->m_pCopyPasteBtn->removeFromParent();
    self->m_pEditSpecialBtn->removeFromParent();
    self->m_pEditGroupBtn->removeFromParent();
    self->m_pEditObjectBtn->removeFromParent();
    self->m_pCopyValuesBtn->removeFromParent();
    self->m_pPasteStateBtn->removeFromParent();
    self->m_pPasteColorBtn->removeFromParent();
    self->m_pEditHSVBtn->removeFromParent();
    self->m_pGoToLayerBtn->removeFromParent();
    self->m_pDeselectBtn->removeFromParent();
}

void loadUICustomizeBtn(EditorPauseLayer* self) {
    auto menu = as<CCMenu*>(self->m_pButton0->getParent());
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    // auto spr = ButtonSprite::create(
    //     "Customize UI", 0, 0, "goldFont.fnt", "GJ_button_01.png", 0, .8f
    // );
    // spr->setScale(.7f);
    // auto btn = CCMenuItemSpriteExtra::create(
    //     spr, self, menu_selector(EditorPauseLayer_CB::onCustomizeUI)
    // );
    // btn->setPosition(0.0f, winSize.height - 60.0f);
    // menu->addChild(btn);
}

