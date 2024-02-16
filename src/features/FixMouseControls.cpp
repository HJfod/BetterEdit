
#include <Geode/modify/EditorUI.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/utils/cocos.hpp>
#include <numbers>
#include <Geode/utils/cocos.hpp>

#undef min
#undef max

using namespace geode::prelude;

#ifdef GEODE_IS_DESKTOP

class $modify(EditorUI) {
    $override
    virtual void scrollWheel(float y, float x) {
        // Disable scrolling during playtest
        if (m_editorLayer->m_playbackMode == PlaybackMode::Playing) {
            return;
        }

        // this is used a lot so store it in a variable
        auto objLayer = m_editorLayer->m_objectLayer;

        // get previous scale of view
        auto prevScale = objLayer->getScale();

        // get the start position of swipe rectangle
        auto swipeStart = objLayer->convertToNodeSpace(m_swipeStart) * prevScale;

        // zoom on control
        if (CCKeyboardDispatcher::get()->getControlKeyPressed()) {
            auto zoom = objLayer->getScale();
            
            // what does this mean? why does it work? i don't know!
            // this is straight from the old BE
            zoom = static_cast<float>(std::pow(
                std::numbers::e,
                std::log(std::max(zoom, .001f)) - y * .01f
            ));
            // zoom limit
            zoom = clamp(zoom, .1f, 10000000.f);

            if (Mod::get()->getSettingValue<bool>("mouse-move-on-zoom")) {
                auto mousePos = getMousePos();
                auto prevPos = objLayer->convertToNodeSpace(mousePos);
                this->updateZoom(zoom);
                auto newPos = objLayer->convertToWorldSpace(prevPos);
                objLayer->setPosition(objLayer->getPosition() + mousePos - newPos);
            }
            else {
                this->updateZoom(zoom);
            }
        }
        // otherwise move screen
        else {
            constexpr float mult = 2.f;

            // move horizontally on shift
            if (CCKeyboardDispatcher::get()->getShiftKeyPressed()) {
                objLayer->setPositionX(
                    objLayer->getPositionX() - y * mult
                );
            }
            // otherwise move as is in vanilla
            else {
                objLayer->setPositionY(
                    objLayer->getPositionY() + y * mult
                );
                // add support for the horizontal trackpad scrolling on mac
                // causes the editor to move sideways when scrolling on windows
                #ifdef GEODE_IS_MACOS
                objLayer->setPositionX(
                    objLayer->getPositionX() + x * mult
                );
                #endif
            }

            // call original but make it not do anything other than update 
            // UI state and stuff
            EditorUI::scrollWheel(.0f, .0f);
        }

        // move swipe when screen is moved

        auto newSwipeStart = objLayer->convertToNodeSpace(m_swipeStart) * prevScale;

        auto rel = swipeStart - newSwipeStart;
        rel = rel * (objLayer->getScale()) / prevScale;

        m_swipeStart = m_swipeStart + rel;
    }
};

#endif
