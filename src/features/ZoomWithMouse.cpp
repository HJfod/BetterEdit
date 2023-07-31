
#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/GJGroundLayer.hpp>
#include <Geode/Loader.hpp>
#include <numbers>

#undef min
#undef max

using namespace geode::prelude;

class $modify(EditorUI) {
    void scrollWheel(float y, float x) override {
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

            this->updateZoom(zoom);

        #ifdef GEODE_IS_WINDOWS
            // move screen toward mouse
            if (Mod::get()->getSettingValue<bool>("mouse-move-on-zoom")) {
                auto winSize = CCDirector::get()->getWinSize();
                auto winSizePx = CCDirector::get()->getOpenGLView()->getViewPortRect();

                auto ratioW = winSize.width  / winSizePx.size.width;
                auto ratioH = winSize.height / winSizePx.size.height;
                auto mpos = CCDirector::get()->getOpenGLView()->getMousePosition();

                // the mouse position is stored from the top-.left while cocos 
                // coordinates are from the bottom-left
                mpos.y = winSizePx.size.height - mpos.y;

                mpos.x *= ratioW;
                mpos.y *= ratioH;

                // relative to window centre
                mpos = mpos - winSize / 2;

                // if zooming out, go in reverse direction
                if (y > .0f) {
                    mpos = -mpos * .5f;
                }

                objLayer->setPosition(
                    objLayer->getPosition() - mpos / std::max(zoom, 5.f)
                );
                this->constrainGameLayerPosition();
            }
        #endif

            // ground width is not automatically updated
            m_editorLayer->m_groundLayer->updateGroundWidth();
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
            // otherwise move vertically as is vanilla
            else {
                objLayer->setPositionY(
                    objLayer->getPositionY() + y * mult
                );
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
