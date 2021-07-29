#include "../../BetterEdit.hpp"
#include "../LevelPercent/levelPercent.hpp"

GDMAKE_HOOK(0x921d0)
void __fastcall EditorUI_scrollWheel(gd::EditorUI* self_, edx_t edx, float amt, float b) {
    // get the actual EditorUI since this function is a virtual that messes it up
    auto self = reinterpret_cast<gd::EditorUI*>(reinterpret_cast<uintptr_t>(self_) - 0xfc);
    auto kb = cocos2d::CCDirector::sharedDirector()->getKeyboardDispatcher();

    if (kb->getControlKeyPressed()) {
        auto zoom = self->m_pEditorLayer->getObjectLayer()->getScale();

        // std::log defaults to base e, and since c++ doesnt have it anywhere, just hardcode it in
        // i mean, whats the worst that can happen, they change a fucking math constant?
        zoom = static_cast<float>(std::pow(2.71828182845904523536, std::log(max(zoom, 0.001f)) - amt * 0.01f));
        // zoom limit
        zoom = max(zoom, .1f);
        zoom = min(zoom, 1000000);  // just in case some dumbass (like me) unlocks
                                    // their scroll wheel and zooms over 1mx

        self->updateZoom(zoom);

        if (!BetterEdit::sharedState()->getDisableMouseZoomMove()) {
            auto winSize = CCDirector::sharedDirector()->getWinSize();
            auto winSizePx = CCDirector::sharedDirector()->getOpenGLView()->getViewPortRect();
            auto ratio_w = winSize.width / winSizePx.size.width; // twitter.com
            auto ratio_h = winSize.height / winSizePx.size.height;

            auto mpos = CCDirector::sharedDirector()->getOpenGLView()->getMousePosition();
            // the mouse position is stored from the top-left while cocos
            // coordinates are from the bottom-left
            mpos.y = winSizePx.size.height - mpos.y;

            mpos.x *= ratio_w;  // scale mouse position to be in
            mpos.y *= ratio_h;  // cocos2d coordinate space

            mpos = mpos - winSize / 2; // relative to window centre

            if (amt > 0.0f)
                mpos = -mpos;

            self->m_pEditorLayer->getObjectLayer()->setPosition(
                self->m_pEditorLayer->getObjectLayer()->getPosition() - mpos / max(zoom, 5.0f)
            );

            self->m_pEditorLayer->getEditorUI()->constrainGameLayerPosition();
        }

        self->m_pEditorLayer->m_pGroundLayer->updateGroundWidth();
    } else {
        auto layer = self->m_pEditorLayer->getObjectLayer();
        constexpr float mult = 2.f;

        if (kb->getShiftKeyPressed())
            layer->setPositionX(layer->getPositionX() - amt * mult);
        else
            layer->setPositionY(layer->getPositionY() + amt * mult);

        GDMAKE_ORIG(self_, edx, 0.f, 0.f); // hehe
    }

    updatePercentLabelPosition(self);
}
