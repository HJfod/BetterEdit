
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>

#undef min
#undef max

using namespace geode::prelude;

class $modify(BEPauseLayer, EditorPauseLayer) {
    $override
    bool init(LevelEditorLayer* lel) {
        if (!EditorPauseLayer::init(lel))
            return false;

        auto menu = this->getChildByID("settings-menu");
        auto spr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn02_001.png");
        spr->setScale(.8f);
        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(BEPauseLayer::onSettings)
        );
        btn->setID("settings"_spr);
        menu->addChild(btn);
        menu->updateLayout();
        
        return true;
    }

    void onSettings(CCObject*) {
        openSettingsPopup(Mod::get());
    }
};
