#include <Geode/modify/ColorSelectLiveOverlay.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>
#include <Geode/binding/ColorAction.hpp>

class $modify(BetterLiveOverlay, ColorSelectLiveOverlay) {
    bool init(ColorAction* base, ColorAction* detail, EffectGameObject* obj) {
        if (!ColorSelectLiveOverlay::init(base, detail, obj))
            return false;
        
        if (base || detail) {
            auto blendingToggle = CCMenuItemToggler::createWithStandardSprites(
                this, menu_selector(BetterLiveOverlay::onBlending), .7f
            );
            blendingToggle->setPosition(154.f, -80.f);
            blendingToggle->toggle(base ? base->m_blending : detail->m_blending);
            blendingToggle->setID("blending-toggle"_spr);
            m_buttonMenu->addChild(blendingToggle);
        }

        return true;
    }

    void onSelectTab(CCObject* sender) {
        ColorSelectLiveOverlay::onSelectTab(sender);
        auto toggle = static_cast<CCMenuItemToggler*>(
            m_buttonMenu->getChildByID("blending-toggle"_spr)
        );
        if (m_detailMode) {
            toggle->toggle(m_detailColorAction->m_blending);
        }
        else {
            toggle->toggle(m_baseColorAction->m_blending);
        }
    }

    void onBlending(CCObject* sender) {
        auto toggle = static_cast<CCMenuItemToggler*>(sender);
        if (m_detailMode) {
            m_detailColorAction->m_blending = !toggle->isToggled();
        }
        else {
            m_baseColorAction->m_blending = !toggle->isToggled();
        }
    }
};
