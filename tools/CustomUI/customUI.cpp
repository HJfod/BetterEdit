#include "customUI.hpp"
#include "../../hooks/EditorPauseLayer.hpp"

bool g_bIsCustomizingEditor = false;
static constexpr const int EDITORDARKBG_TAG = 0x500;

bool isCustomizingEditor() {
    return g_bIsCustomizingEditor;
}

void stopCustomizingEditor() {
    g_bIsCustomizingEditor = false;
    auto lel = LevelEditorLayer::get();
    if (!lel) return;
    lel->removeChildByTag(EDITORDARKBG_TAG);
    as<CCMenu*>(
        lel->m_pEditorUI->m_pSwipeBtn->getParent()
    )->setOpacity(255);
    as<CCMenu*>(
        lel->m_pEditorUI->m_pEditGroupBtn->getParent()
    )->setOpacity(255);
    lel->m_pEditorUI->m_pPositionSlider->m_pGroove->setOpacity(255);
    lel->m_pEditorUI->m_pPositionSlider->m_pSliderBar->setOpacity(255);
}

void EditorPauseLayer_CB::onCustomizeUI(CCObject*) {
    g_bIsCustomizingEditor = true;

    auto bg = CCLayerColor::create({ 0, 0, 0, 220 });
    this->m_pEditorLayer->addChild(bg, 99, EDITORDARKBG_TAG);
    as<CCMenu*>(
        this->m_pEditorLayer->m_pEditorUI->m_pSwipeBtn->getParent()
    )->setOpacity(50);
    as<CCMenu*>(
        this->m_pEditorLayer->m_pEditorUI->m_pEditGroupBtn->getParent()
    )->setOpacity(50);
    this->m_pEditorLayer->m_pEditorUI->m_pPositionSlider->m_pGroove->setOpacity(50);
    this->m_pEditorLayer->m_pEditorUI->m_pPositionSlider->m_pSliderBar->setOpacity(50);

    this->m_pEditorLayer->m_pEditorUI->m_pEditButtonBar->setVisible(false);

    this->onResume(nullptr);
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

