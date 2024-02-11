#include <Geode/modify/EditorUI.hpp>
#include <Geode/loader/Mod.hpp>
#include <numbers>

using namespace geode::prelude;

#ifdef GEODE_IS_MOBILE

// thank you mat matcool

class $modify(EditorUI) {
    std::unordered_set<Ref<CCTouch>> m_touches;
    float m_initialDistance = 0.f;
    float m_initialScale = 1.f;
    CCPoint m_touchMidPoint;

    $override
    bool ccTouchBegan(CCTouch* touch, CCEvent* event) override {
        if (!Mod::get()->getSettingValue<bool>("pinch-to-zoom")) {
            return EditorUI::ccTouchBegan(touch, event);
        }
        if (m_editorLayer->m_playbackMode != PlaybackMode::Playing && m_fields->m_touches.size() == 1) {
            auto firstTouch = *m_fields->m_touches.begin();

            auto firstLoc = firstTouch->getLocation();
            auto secondLoc = touch->getLocation();

            m_fields->m_touchMidPoint = (firstLoc + secondLoc) / 2.f;
            // save current zoom level
            m_fields->m_initialScale = m_editorLayer->m_objectLayer->getScale();
            // distance between the two touches
            m_fields->m_initialDistance = firstLoc.getDistance(secondLoc);

            m_fields->m_touches.insert(touch);
            return true;
        }
        else if (EditorUI::ccTouchBegan(touch, event)) {
            m_fields->m_touches.insert(touch);
            return true;
        }
        return false;
    }

    $override
    void ccTouchMoved(CCTouch* touch, CCEvent* event) override {
        if (!Mod::get()->getSettingValue<bool>("pinch-to-zoom")) {
            return EditorUI::ccTouchMoved(touch, event);
        }
        if (m_editorLayer->m_playbackMode != PlaybackMode::Playing && m_fields->m_touches.size() == 2) {
            auto objLayer = m_editorLayer->m_objectLayer;

            auto it = m_fields->m_touches.begin();
            auto firstTouch = *it;
            ++it;
            auto secondTouch = *it;

            auto firstLoc = firstTouch->getLocation();
            auto secondLoc = secondTouch->getLocation();

            auto center = (firstLoc + secondLoc) / 2.f;
            auto distNow = firstLoc.getDistance(secondLoc);
            
            auto const mult = m_fields->m_initialDistance / distNow;
            auto zoom = std::max(m_fields->m_initialScale / mult, 0.1f);
            // safety measure, nan zoom can really mess up gd
            if (std::isnan(zoom) || std::isinf(zoom)) {
                this->updateZoom(1.f);
                zoom = 1.f;
            }
            else {
                this->updateZoom(zoom);
            }

            // pan the level if the center point moves
            auto centerDiff = m_fields->m_touchMidPoint - center;
            objLayer->setPosition(objLayer->getPosition() - centerDiff);

            m_fields->m_touchMidPoint = center;
        }
        else {
            EditorUI::ccTouchMoved(touch, event);
        }
    }

    $override
    void ccTouchEnded(CCTouch* touch, CCEvent* event) override {
        if (!Mod::get()->getSettingValue<bool>("pinch-to-zoom")) {
            return EditorUI::ccTouchEnded(touch, event);
        }
        EditorUI::ccTouchEnded(touch, event);
        m_fields->m_touches.erase(touch);
    }

    $override
    void ccTouchCancelled(CCTouch* touch, CCEvent* event) override {
        if (!Mod::get()->getSettingValue<bool>("pinch-to-zoom")) {
            return EditorUI::ccTouchCancelled(touch, event);
        }
        EditorUI::ccTouchCancelled(touch, event);
        m_fields->m_touches.erase(touch);
    }
};

#endif
