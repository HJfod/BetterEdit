#include <Geode/modify/EditorUI.hpp>
#include <utils/HandleUIHide.hpp>

using namespace geode::prelude;

class $modify(HideButtonUI, EditorUI) {
    struct Fields {
        CCMenuItemToggler* hideToggle = nullptr;
    };
    
    $override
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;
        
        if (auto menu = this->getChildByID("undo-menu")) {
            auto unhideSpr = CircleButtonSprite::createWithSpriteFrameName(
                "eye-off.png"_spr, 1.f, CircleBaseColor::Gray, CircleBaseSize::Tiny
            );
            unhideSpr->setOpacity(105);
            static_cast<CCSprite*>(unhideSpr->getTopNode())->setOpacity(105);
            auto hideSpr   = CircleButtonSprite::createWithSpriteFrameName(
                "eye-on.png"_spr, 1.f, CircleBaseColor::Green, CircleBaseSize::Tiny
            );

            m_fields->hideToggle = CCMenuItemToggler::create(
                hideSpr, unhideSpr, this, menu_selector(HideButtonUI::onHide)
            );
            m_fields->hideToggle->setID("hide-ui-toggle"_spr);
            m_fields->hideToggle->m_notClickable = true;
            menu->addChild(m_fields->hideToggle);
            menu->updateLayout();
        }

        return true;
    }

    $override
    void showUI(bool show) {
        EditorUI::showUI(show);
        if (m_fields->hideToggle) {
            m_fields->hideToggle->toggle(!show);
            m_fields->hideToggle->setVisible(m_editorLayer->m_playbackMode != PlaybackMode::Playing);
        }
    }

    void onHide(CCObject*) {
        this->showUI(m_fields->hideToggle->isToggled());
    }
};
