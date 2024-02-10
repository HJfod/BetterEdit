
#include <Geode/modify/EditorPauseLayer.hpp>

#undef min
#undef max

using namespace geode::prelude;

class $modify(ScrollBackLayer, EditorPauseLayer) {
    $override
    bool init(LevelEditorLayer* lel) {
        if (!EditorPauseLayer::init(lel))
            return false;
        
        auto winSize = CCDirector::get()->getWinSize();

        auto menu = this->getChildByID("small-actions-menu");
        
        auto resetScrollSpr = ButtonSprite::create(
            "Reset\nScroll", 30, 0, .4f, true,
            "bigFont.fnt", "GJ_button_04.png", 30.f
        );
        resetScrollSpr->setScale(.8f);
        auto farFromContentBtn = CCMenuItemSpriteExtra::create(
            resetScrollSpr, this, menu_selector(ScrollBackLayer::onResetScroll)
        );
        farFromContentBtn->setID("far-from-content-button"_spr);
        menu->insertBefore(farFromContentBtn, nullptr);

        menu->updateLayout();
        
        return true;
    }

    void onResetScroll(CCObject*) {
        m_editorLayer->m_objectLayer->setPosition(ccp(0, 0));
        m_editorLayer->m_editorUI->updateZoom(1.f);
    }
};
