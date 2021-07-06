#include <GDMake.h>
#include <GUI/CCControlExtension/CCScale9Sprite.h>
#include "../utils.hpp"
#include "passTouch.hpp"
#include "../tools/GroupIDFilter/groupfilter.hpp"
#include "../tools/GridSize/gridButton.hpp"
#include "../tools/LevelPercent/levelPercent.hpp"
#include "../BetterEdit.hpp"
#include <thread>
#include "ScaleTextDelegate.hpp"

using namespace gdmake;
using namespace cocos2d;

#define CATCH_NULL(x) if (x) x

static constexpr const int LAYERINPUT_TAG = 6978;
static constexpr const int LAYERINPUTBG_TAG = 6977;
static constexpr const int ZOOMLABEL_TAG = 6976;

char* g_clipboard;

void showErrorMessages() {
    std::cout << "thread!\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::cout << "showy\n";

    for (auto errMsg : BetterEdit::sharedState()->getErrors())
        gd::FLAlertLayer::create(
            nullptr,
            "Error",
            "OK", nullptr,
            errMsg
        )->show();

    std::cout << "bye\n";
    
    std::terminate();
}

class EUITextDelegate : public cocos2d::CCNode, public gd::TextInputDelegate {
    public:
        gd::EditorUI* m_pEditorUI;

        virtual void textChanged(gd::CCTextInputNode* input) override {
            if (input->getString() && strlen(input->getString()))
                this->m_pEditorUI->m_pEditorLayer->setCurrentLayer(strToInt(input->getString()));
            else
                this->m_pEditorUI->m_pEditorLayer->setCurrentLayer(-1);
        }

        static EUITextDelegate* create(gd::EditorUI* ui) {
            auto ret = new EUITextDelegate();

            if (ret && ret->init()) {
                ret->m_pEditorUI = ui;
                ret->autorelease();
                return ret;
            }

            CC_SAFE_DELETE(ret);
            return nullptr;
        }
};

GDMAKE_HOOK(0x886b0)
void __fastcall EditorUI_onGoToLayer(gd::EditorUI* self, edx_t edx, cocos2d::CCObject* pSender) {
    GDMAKE_ORIG_V(self, edx, pSender);

    auto i = self->getChildByTag(LAYERINPUT_TAG);

    self->m_pCurrentLayerLabel->setVisible(false);

    if (i)
        reinterpret_cast<gd::CCTextInputNode*>(i)->setString(
            self->m_pCurrentLayerLabel->getString()
        );
}

GDMAKE_HOOK(0x907b0)
bool __fastcall EditorUI_ccTouchBegan(gd::EditorUI* self, edx_t edx, cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    auto self_ = reinterpret_cast<gd::EditorUI*>(reinterpret_cast<uintptr_t>(self) - 0xEC);
    auto i = self_->getChildByTag(LAYERINPUT_TAG);

    editorHasBeenTouched(true);

    g_bHoldingDownTouch = true;
    
    if (i) {
        auto inp = reinterpret_cast<gd::CCTextInputNode*>(i);
        auto isize = inp->getScaledContentSize();

        auto rect = cocos2d::CCRect {
            inp->getPositionX() - isize.width / 2,
            inp->getPositionY() - isize.height / 2,
            isize.width,
            isize.height
        };

        if (!rect.containsPoint(touch->getLocation()))
            reinterpret_cast<gd::CCTextInputNode*>(i)->getTextField()->detachWithIME();
        else
            return true;
    }
    
    if (pointIntersectsControls(self_, touch, event))
        return true;
    
    if (BetterEdit::sharedState()->m_bHookConflictFound)
        BetterEdit::showHookConflictMessage();

    return GDMAKE_ORIG(self, edx, touch, event);
}

GDMAKE_HOOK(0x911a0)
void __fastcall EditorUI_ccTouchEnded(gd::EditorUI* self, edx_t edx, cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    g_bHoldingDownTouch = false;

    GDMAKE_ORIG_V(self, edx, touch, event);
}

GDMAKE_HOOK(0x8d7e0)
void __fastcall EditorUI_onGroupDown(gd::EditorUI* self, edx_t edx, cocos2d::CCObject* pSender) {
    GDMAKE_ORIG(self, edx, pSender);

    auto i = self->getChildByTag(LAYERINPUT_TAG);

    self->m_pCurrentLayerLabel->setVisible(false);

    if (i)
        reinterpret_cast<gd::CCTextInputNode*>(i)->setString(
            self->m_pCurrentLayerLabel->getString()
        );
}

GDMAKE_HOOK(0x8d780)
void __fastcall EditorUI_onGroupUp(gd::EditorUI* self, edx_t edx, cocos2d::CCObject* pSender) {
    GDMAKE_ORIG(self, edx, pSender);

    auto i = self->getChildByTag(LAYERINPUT_TAG);

    self->m_pCurrentLayerLabel->setVisible(false);

    if (i)
        reinterpret_cast<gd::CCTextInputNode*>(i)->setString(
            self->m_pCurrentLayerLabel->getString()
        );
}

GDMAKE_HOOK(0x88790)
void __fastcall EditorUI_onGoToBaseLayer(gd::EditorUI* self, edx_t edx, cocos2d::CCObject* pSender) {
    GDMAKE_ORIG(self, edx, pSender);

    auto i = self->getChildByTag(LAYERINPUT_TAG);

    self->m_pCurrentLayerLabel->setVisible(false);

    if (i)
        reinterpret_cast<gd::CCTextInputNode*>(i)->setString(
            self->m_pCurrentLayerLabel->getString()
        );
}

GDMAKE_HOOK(0x874f0)
void __fastcall EditorUI_updatePlaybackBtn(gd::EditorUI* self) {
    // if (BetterEdit::getPulseObjectsInEditor()) {
    //     auto pulse = FMODAudioEngine::sharedEngine()->m_fPulse1 + 
    //         FMODAudioEngine::sharedEngine()->m_fPulse2 +
    //         FMODAudioEngine::sharedEngine()->m_fPulse3;
    //     pulse /= 3.f;

    //     GameManager::sharedState()->getEditorLayer()->getAllObjects();

    //     patchBytes(0x23b56, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
    // } else
    //     patchBytes(0x23b56, { 0x0f, 0x84, 0x23, 0x01, 0x00, 0x00 });

    GDMAKE_ORIG_V(self);
}

GDMAKE_HOOK(0x76090)
void __fastcall EditorUI_destructorHook(gd::EditorUI* self) {
    auto addr = reinterpret_cast<uintptr_t>(self) + 0x2D0;
    auto str_len = *reinterpret_cast<size_t*>(addr + 16);
    if (str_len) {
        char* str_buf;
        if (str_len < 16) {
            // string is small enough to be directly here
            str_buf = reinterpret_cast<char*>(addr);
        } else {
            str_buf = *reinterpret_cast<char**>(addr);
        }
        g_clipboard = reinterpret_cast<char*>(realloc(g_clipboard, str_len + 1));
        memcpy(g_clipboard, str_buf, str_len + 1);
    }

    editorHasBeenTouched(false);

    return GDMAKE_ORIG_V(self);
}

GDMAKE_HOOK(EditorUI::init)
bool __fastcall EditorUI_init(gd::EditorUI* self, edx_t edx, gd::GJGameLevel* lvl) {
    if (!GDMAKE_ORIG(self, edx, lvl))
        return false;
    
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    auto ed = EUITextDelegate::create(self);

    self->m_pCurrentLayerLabel->setVisible(false);

    auto spr = cocos2d::extension::CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );

    spr->setScale(.3f);
    spr->setColor({ 0, 0, 0 });
    spr->setOpacity(100);
    spr->setContentSize({ 115.0f, 75.0f });
    spr->setTag(LAYERINPUTBG_TAG);
    spr->setPosition(self->m_pCurrentLayerLabel->getPosition());

    auto eLayerInput = gd::CCTextInputNode::create("All", ed, "bigFont.fnt", 40.0f, 30.0f);

    eLayerInput->setPosition(self->m_pCurrentLayerLabel->getPosition());
    eLayerInput->setLabelPlaceholderColor({ 120, 120, 120 });
    eLayerInput->setAllowedChars("0123456789");
    eLayerInput->setAnchorPoint({ 0, 0 });
    eLayerInput->setScale(.7f);
    eLayerInput->setDelegate(ed);
    eLayerInput->setTag(LAYERINPUT_TAG);
    eLayerInput->setString(self->m_pCurrentLayerLabel->getString());

    self->addChild(spr);
    self->addChild(eLayerInput);

    self->addChild(ed);

    auto currentZoomLabel = CCLabelBMFont::create("", "bigFont.fnt");
    currentZoomLabel->setScale(.5f);
    currentZoomLabel->setPosition(winSize.width / 2, winSize.height - 60.0f);
    currentZoomLabel->setTag(ZOOMLABEL_TAG);
    currentZoomLabel->setOpacity(0);
    currentZoomLabel->setZOrder(99999);
    self->addChild(currentZoomLabel);

    setupGroupFilterButton(self);
    loadGridButtons(self);
    loadSliderPercent(self);

    if (g_clipboard && g_clipboard[0]) {
        auto clipboard = reinterpret_cast<uintptr_t>(self) + 0x2D0;
        auto len = strlen(g_clipboard);
        *reinterpret_cast<size_t*>(clipboard + 16) = len; // size
        *reinterpret_cast<size_t*>(clipboard + 20) = max(len, 15); // capacity
        if (len <= 15) {
            memcpy(reinterpret_cast<char*>(clipboard), g_clipboard, len + 1);
        } else {
            void* newb = malloc(len + 1);
            memcpy(newb, g_clipboard, len + 1);
            *reinterpret_cast<void**>(clipboard) = newb;
        }
        self->updateButtons();
    }

    BetterEdit::sharedState()->m_bHookConflictFound = false;

    return true;
}

GDMAKE_HOOK(0x87180)
void __fastcall EditorUI_showUI(gd::EditorUI* self, edx_t edx, bool show) {
    GDMAKE_ORIG_V(self, edx, show);

    self->m_pCurrentLayerLabel->setVisible(false);

    CATCH_NULL(self->getChildByTag(LAYERINPUT_TAG))->setVisible(show);
    CATCH_NULL(self->getChildByTag(LAYERINPUTBG_TAG))->setVisible(show);
    CATCH_NULL(self->m_pCopyBtn->getParent()->getChildByTag(7777))->setVisible(show);
    showGridButtons(self, show);
    showPositionLabel(self, show);
}

GDMAKE_HOOK(0x878a0)
void __fastcall EditorUI_updateZoom(gd::EditorUI* self) {
    GDMAKE_ORIG_V(self);

    float zoom;
    __asm { movss zoom, xmm1 }

    if (!BetterEdit::getDisableZoomText()) {
        auto zLabel = as<CCLabelBMFont*>(self->getChildByTag(ZOOMLABEL_TAG));

        if (zLabel) {
            zLabel->setString(
                ("Zoom: "_s +
                BetterEdit::formatToString(
                    self->m_pEditorLayer->getObjectLayer()->getScale(), 2u
                ) +"x"_s
            ).c_str());
            zLabel->setOpacity(255);
            zLabel->stopAllActions();
            zLabel->runAction(CCSequence::create(CCArray::create(
                CCDelayTime::create(.5f),
                CCFadeOut::create(.5f),
                nullptr
            )));
        }
    }

    updatePercentLabelPosition(self);
}

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

GDMAKE_HOOK(0x91a30)
void __fastcall EditorUI_keyDown(EditorUI* self_, edx_t edx, enumKeyCodes key) {
    auto kb = cocos2d::CCDirector::sharedDirector()->getKeyboardDispatcher();
    
    auto self = reinterpret_cast<gd::EditorUI*>(reinterpret_cast<uintptr_t>(self_) - 0xf8);

    if (key == enumKeyCodes::KEY_A && kb->getControlKeyPressed())
        self->selectAll();
    else
        GDMAKE_ORIG_V(self_, edx, key);
}

// Credits to Alk1m123 (https://github.com/altalk23) for this scale fix
// this lets you scale multiple objects without it fucking up the position
GDMAKE_HOOK(0x8f150)
void __fastcall EditorUI_scaleObjects(gd::EditorUI* self, edx_t, CCArray* objs, CCPoint centerPos) {
    float scale;
    __asm movss scale, xmm2;
    CCObject* obj;

    // maybe add some scale anchor point feature, as itd
    // only require changing the centerPos here

    // prevent the scale from being 0
    // as that can cause weird behaviour
    if (scale > -0.01f && scale < 0.01f) {
        scale = std::copysign(0.01f, scale);
    }
    bool lockPos = false;
    if (self->m_pScaleControl) {
        auto fancyWidget = dynamic_cast<ScaleTextDelegate*>(self->m_pScaleControl->getChildByTag(7777));
        if (fancyWidget)
            lockPos = fancyWidget->m_bLockPosEnabled;
    }
    CCARRAY_FOREACH(objs, obj) {
        auto gameObj = reinterpret_cast<gd::GameObject*>(obj);
        auto pos = gameObj->getPosition();
        float newScale = gameObj->m_fMultiScaleMultiplier * scale;
        float newMultiplier = newScale / gameObj->m_fScale;
        auto newPos = (pos - centerPos) * newMultiplier + (centerPos - pos);
        
        // this is just GameObject::updateCustomScale
        // although that does some rounding so its stupid
        gameObj->m_fScale = newScale;
        gameObj->setRScale(1.f);
        gameObj->m_unk2B0 = true;
        gameObj->m_bIsObjectRectDirty = true;

        if (!lockPos)
            self->moveObject(gameObj, newPos);
    }
}