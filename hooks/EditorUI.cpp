#include "../BetterEdit.hpp"
#include "passTouch.hpp"
#include "ScaleTextDelegate.hpp"
#include "../tools/GroupIDFilter/groupfilter.hpp"
#include "../tools/GridSize/gridButton.hpp"
#include "../tools/LevelPercent/levelPercent.hpp"
#include "../tools/GlobalClipboard/clipboardHook.hpp"
#include "../tools/EditorLayerInput/editorLayerInput.hpp"
#include <thread>

using namespace gdmake;
using namespace cocos2d;

static constexpr const int ZOOMLABEL_TAG = 6976;
static constexpr const int TOGGLEUI_TAG = 6979;

bool g_showUI = true;
bool g_hasResetObjectsScale = true;

// TODO: Clean up this whole file because man is it ugly
// TODO: (move shit out to their own files)

// TODO: have this somewhere
// patchBytes(0x23b56, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
// you dont really have to patch it back to the original, game works fine with
// the patch always on

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

class EditorUI_CB : public EditorUI {
    public:
        void onToggleShowUI(CCObject*) {
            this->showUI(!g_showUI);

            auto btn = as<CCMenuItemSpriteExtra*>(this->m_pButton4->getParent()->getChildByTag(TOGGLEUI_TAG));
            if (btn) {
                btn->setVisible(true);

                auto spr = as<ButtonSprite*>(btn->getNormalImage());

                spr->updateBGImage(g_showUI ? "GJ_button_01.png" : "GJ_button_06.png");
                spr->setString(g_showUI ? "Hide" : "Show");
            }
        }
};

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

GDMAKE_HOOK(0x76090)
void __fastcall EditorUI_destructorHook(gd::EditorUI* self) {
    saveClipboard(self);

    editorHasBeenTouched(false);

    return GDMAKE_ORIG_V(self);
}

// this exists entirely because dynamic_cast<gd::GameObject*> doesnt work
gd::GameObject* castToGameObject(CCObject* obj) {
    if (obj != nullptr) {
        const auto vtable = *reinterpret_cast<uintptr_t*>(obj) - gd::base;
        // GameObject and RingObject respectively
        if (vtable == 0x29A514 || vtable == 0x2E390C) {
            return reinterpret_cast<gd::GameObject*>(obj);
        }
    }
    return nullptr;
}

class EditorUIPulse : public gd::EditorUI {
public:
    void updateObjectsPulse(float dt) {
        // too lazy to add these to gd.h
        // theyre isMusicPlaying and isPlaybackMode
        if (
            (*reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + 0x130) || m_pEditorLayer->m_bIsPlaybackMode) &&
            BetterEdit::getPulseObjectsInEditor()
        ) {
            g_hasResetObjectsScale = false;

            // incredible
            const auto arr = m_pEditorLayer->m_pSpriteBatch29->getChildren();
            const auto arr2 = m_pEditorLayer->m_pSpriteBatch28->getChildren();

            auto fmod = gd::FMODAudioEngine::sharedEngine();
            auto pulse = fmod->m_fPulse1 + fmod->m_fPulse2 + fmod->m_fPulse3;
            pulse /= 3.f;

            const auto f = [&](CCObject* obj_) {
                auto obj = castToGameObject(obj_);
                if (obj != nullptr && obj->m_unk32C)
                    obj->setRScale(pulse);
            };
            CCARRAY_FOREACH_B(arr, obj_)
                f(obj_);
            CCARRAY_FOREACH_B(arr2, obj_)
                f(obj_);

        } else if (!g_hasResetObjectsScale) {
            g_hasResetObjectsScale = true;

            const auto arr = m_pEditorLayer->getAllObjects();
            CCObject* obj_;

            CCARRAY_FOREACH(arr, obj_) {
                auto obj = castToGameObject(obj_);
                if (obj != nullptr && obj->m_unk32C)
                    obj->setRScale(1.f);
            }
        }
    }
};

GDMAKE_HOOK(EditorUI::init)
bool __fastcall EditorUI_init(gd::EditorUI* self, edx_t edx, gd::GJGameLevel* lvl) {
    if (!GDMAKE_ORIG(self, edx, lvl))
        return false;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto currentZoomLabel = CCLabelBMFont::create("", "bigFont.fnt");
    currentZoomLabel->setScale(.5f);
    currentZoomLabel->setPosition(winSize.width / 2, winSize.height - 60.0f);
    currentZoomLabel->setTag(ZOOMLABEL_TAG);
    currentZoomLabel->setOpacity(0);
    currentZoomLabel->setZOrder(99999);
    self->addChild(currentZoomLabel);

    self->m_pButton4->getParent()->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(
                CCMenuItemSpriteExtra::create(
                    CCNodeConstructor<ButtonSprite*>()
                        .fromButtonSprite("Hide", "GJ_button_01.png", "bigFont.fnt")
                        .scale(.6f)
                        .done(),
                    self,
                    (SEL_MenuHandler)&EditorUI_CB::onToggleShowUI
                )
            )
            .tag(TOGGLEUI_TAG)
            .move(-100.0f, 140.0f)
            .done()
    );

    loadEditorLayerInput(self);
    setupGroupFilterButton(self);
    loadGridButtons(self);
    loadSliderPercent(self);
    loadClipboard(self);
    
    BetterEdit::sharedState()->m_bHookConflictFound = false;

    g_hasResetObjectsScale = true;
    self->schedule(schedule_selector(EditorUIPulse::updateObjectsPulse));

    return true;
}

GDMAKE_HOOK(0x87180)
void __fastcall EditorUI_showUI(gd::EditorUI* self, edx_t edx, bool show) {
    GDMAKE_ORIG_V(self, edx, show);

    g_showUI = show;

    self->m_pTabsMenu->setVisible(show);
    CATCH_NULL(self->m_pButton4->getParent()->getChildByTag(TOGGLEUI_TAG))->setVisible(show);
    CATCH_NULL(self->m_pCopyBtn->getParent()->getChildByTag(7777))->setVisible(show);
    showGridButtons(self, show);
    showLayerControls(self, show);
    // showPositionLabel(self, show);
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

