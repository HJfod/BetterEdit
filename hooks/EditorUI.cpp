#include "../BetterEdit.hpp"
#include "passTouch.hpp"
#include "ScaleTextDelegate.hpp"
#include "../tools/GroupIDFilter/groupfilter.hpp"
#include "../tools/GridSize/gridButton.hpp"
#include "../tools/LevelPercent/levelPercent.hpp"
#include "../tools/GlobalClipboard/clipboardHook.hpp"
#include "../tools/EditorLayerInput/editorLayerInput.hpp"
#include "../tools/RepeatPaste/repeatPaste.hpp"
#include "../tools/AutoSave/autoSave.hpp"
#include "../tools/AutoSave/Backup/BackupViewLayer.hpp"
#include "../tools/VisibilityTab/loadVisibilityTab.hpp"
#include "../tools/CustomKeybinds/KeybindManager.hpp"
#include "../tools/CustomKeybinds/loadEditorKeybindIndicators.hpp"
#include "../tools/CustomUI/customUI.hpp"
#include "EditorPauseLayer.hpp"
#include "EditorUI.hpp"
#include <thread>
#include <chrono>

using namespace gdmake;
using namespace cocos2d;

static constexpr const int ZOOMLABEL_TAG = 6976;
static constexpr const int TOGGLEUI_TAG = 6979;
static constexpr const int VIEWMODE_BACKBTN_TAG = 59305;

bool g_showUI = true;
bool g_uiIsVisible = true;
bool g_hasResetObjectsScale = true;
std::chrono::time_point<std::chrono::system_clock> g_lastTouchTime
    = std::chrono::system_clock::now();

// TODO: Clean up this whole file because man is it ugly
// TODO: (move shit out to their own files)

void toggleShowUI(EditorUI* self) {
    self->showUI(!g_uiIsVisible);
}

CCPoint getShowButtonPosition(EditorUI* self) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto ratio = winSize.width / winSize.height;

    if (ratio > 1.5f)
        return {
            self->m_pTrashBtn->getPositionX() + 50.0f,
            self->m_pTrashBtn->getPositionY()
        };
    
    return {
        self->m_pPlaybackBtn->getPositionX() + 45.0f,
        self->m_pPlaybackBtn->getPositionY()
    };
}

void updateToggleButtonSprite(CCMenuItemSpriteExtra* btn) {
    auto spr = btn->getNormalImage();
    spr->retain();

    btn->setNormalImage(
        CCNodeConstructor()
            .fromBESprite(g_showUI ?
                "BE_eye-on-btn.png" : 
                "BE_eye-off-btn.png")
            .scale(spr->getScale())
            .csize(spr->getContentSize())
            .done()
    );

    btn->getNormalImage()->setPosition(spr->getPosition());
    btn->getNormalImage()->setAnchorPoint(spr->getAnchorPoint());
    btn->getNormalImage()->setScale(spr->getScale());
    btn->setOpacity(g_showUI ? 255 : 90);

    spr->release();
}

void EditorUI_CB::onToggleShowUI(CCObject*) {
    g_showUI = !g_showUI;
    this->showUI(g_showUI);

    auto btn = as<CCMenuItemSpriteExtra*>(this->m_pSwipeBtn->getParent()->getChildByTag(TOGGLEUI_TAG));

    if (btn && !g_showUI)
        updateToggleButtonSprite(btn);
}

void EditorUI_CB::onExitViewMode(CCObject*) {
    CCDirector::sharedDirector()->popSceneWithTransition(
        0.5f, cocos2d::kPopTransitionFade
    );
}

bool touchIntersectsInput(CCNode* input, CCTouch* touch) {
    if (!input)
        return false;

    auto inp = reinterpret_cast<gd::CCTextInputNode*>(input);
    auto isize = inp->getScaledContentSize();

    auto rect = cocos2d::CCRect {
        inp->getPositionX() - isize.width / 2,
        inp->getPositionY() - isize.height / 2,
        isize.width,
        isize.height
    };

    if (!rect.containsPoint(input->getParent()->convertTouchToNodeSpace(touch))) {
        reinterpret_cast<gd::CCTextInputNode*>(input)->getTextField()->detachWithIME();
        return false;
    } else
        return true;
}

GDMAKE_HOOK(0x907b0)
bool __fastcall EditorUI_ccTouchBegan(EditorUI* self, edx_t edx, CCTouch* touch,CCEvent* event) {
    auto self_ = reinterpret_cast<gd::EditorUI*>(reinterpret_cast<uintptr_t>(self) - 0xEC);

    g_bHoldingDownTouch = true;
    
    if (UIManager::get()->isCustomizing())
        return true;

    if (touchIntersectsInput(self_->getChildByTag(LAYERINPUT_TAG), touch))
        return true;
    if (touchIntersectsInput(getGridButtonParent(self_)->getChildByTag(ZOOMINPUT_TAG), touch))
        return true;

    if (pointIntersectsControls(self_, touch, event))
        return true;
    
    if (BetterEdit::sharedState()->m_bHookConflictFound)
        BetterEdit::showHookConflictMessage();
    
    bool swipe = KeybindManager::get()->isModifierPressed("gd.edit.swipe_modifier");

    patch(0x90951, swipe ? 
        std::vector<uint8_t> { 0x90, 0x90, 0x90, 0x90,  0xe9, 0x6b, 0x02, 0x00, 0x00, 0x00 } :
        std::vector<uint8_t> { 0x90, 0x90, 0x90, 0x90,  0x90, 0x90, 0x90, 0x90, 0x90, 0x90, },
        false, true
    );
    patch(0x90be1, swipe ? 
        std::vector<uint8_t> { 0x90, 0x90, 0x90, 0x90,  0x90, 0x90, } :
        std::vector<uint8_t> { 0x90, 0x90, 0x90, 0x90,  0xeb, 0x23, },
        false, true
    );
    patch(0x90c20, swipe ? 
        std::vector<uint8_t> { 0x90, 0x90, 0x90, 0x90,  0xeb, 0x04, } :
        std::vector<uint8_t> { 0x90, 0x90, 0x90, 0x90,  0x90, 0x90, },
        false, true
    );

    bool move = KeybindManager::get()->isModifierPressed("gd.edit.move_modifier");
    
    self_->m_bSpaceKeyPressed = move;
    if (!self_->m_pEditorLayer->m_bIsPlaybackMode)
        self_->m_bMoveModifier = move;

    return GDMAKE_ORIG(self, edx, touch, event);
}

GDMAKE_HOOK(0x90cd0)
void __fastcall EditorUI_ccTouchMoved(EditorUI* self_, edx_t edx, CCTouch* touch, CCEvent* event) {
    auto self = reinterpret_cast<EditorUI*>(reinterpret_cast<uintptr_t>(self_) - 0xEC);

    if (UIManager::get()->isCustomizing()) return;

    float prevScale = self->m_pEditorLayer->m_pObjectLayer->getScale();
    auto swipeStart =
        self->m_pEditorLayer->m_pObjectLayer->convertToNodeSpace(self->m_obSwipeStart) * prevScale;
        
    bool swipe = KeybindManager::get()->isModifierPressed("gd.edit.swipe_modifier");

    patch(0x90ff6, swipe ? 
        std::vector<uint8_t> { 0x90, 0x90, 0x90, 0x90,  0xeb, 0x0d, } :
        std::vector<uint8_t> { 0x90, 0x90, 0x90, 0x90,  0x90, 0x90, },
        false, true
    );

    GDMAKE_ORIG_V(self_, edx, touch, event);

    auto nSwipeStart = 
        self->m_pEditorLayer->m_pObjectLayer->convertToNodeSpace(self->m_obSwipeStart) * prevScale;
    
    auto rel = swipeStart - nSwipeStart;
    rel = rel * (self->m_pEditorLayer->m_pObjectLayer->getScale() / prevScale);

    if (BetterEdit::getEnableRelativeSwipe())
        self->m_obSwipeStart = self->m_obSwipeStart + rel;
}

GDMAKE_HOOK(0x911a0)
void __fastcall EditorUI_ccTouchEnded(
    EditorUI* self,
    edx_t edx,
    CCTouch* touch,
    CCEvent* event
) {
    g_bHoldingDownTouch = false;

    if (UIManager::get()->isCustomizing()) return;

    auto now = std::chrono::system_clock::now();

    auto self_ = as<EditorUI*>(as<uintptr_t>(self) - 0xEC);
    if (
        !BetterEdit::getDisableDoubleClick() &&
        !self_->m_pEditorLayer->m_bIsPlaybackMode &&
        std::chrono::duration_cast<std::chrono::milliseconds>(
            now - g_lastTouchTime
        ).count() < KeybindManager::get()->getDoubleClickInterval()
    ) {

        if (
            CCDirector::sharedDirector()->getKeyboardDispatcher()
            ->getControlKeyPressed()
        ) {
            self_->editGroup(nullptr);
        } else if (
            CCDirector::sharedDirector()->getKeyboardDispatcher()
            ->getShiftKeyPressed()
        ) {
            self_->editObject2(nullptr);
        } else {
            self_->editObject(nullptr);
        }
    
        self_->m_bTouchDown = false;
        self_->m_nTouchID = -1;
        self_->stopActionByTag(0x7b);

        g_lastTouchTime = now;
        
        return;
    }

    g_lastTouchTime = now;

    GDMAKE_ORIG_V(self, edx, touch, event);
}

GDMAKE_HOOK(0x78860)
void __fastcall EditorUI_clickOnPosition(EditorUI* self, edx_t edx, CCPoint point) {
    if (!BetterEdit::isEditorViewOnlyMode())
        return GDMAKE_ORIG_V(self, edx, point);
}

GDMAKE_HOOK(0x76090)
void __fastcall EditorUI_destructorHook(gd::EditorUI* self) {
    saveClipboard(self);
    resetSliderPercent(self);
    clearEditorKeybindIndicators(self);
    getAutoSaveTimer(self)->resetTimer();

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

        auto volume = FMODAudioEngine::sharedEngine()->m_fBackgroundMusicVolume;
        if (
            (*reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + 0x130)
            || m_pEditorLayer->m_bIsPlaybackMode) &&
            volume &&
            BetterEdit::getPulseObjectsInEditor()
        ) {
            g_hasResetObjectsScale = false;

            auto fmod = FMODAudioEngine::sharedEngine();
            auto pulse = fmod->m_fPulse1 + fmod->m_fPulse2 + fmod->m_fPulse3;
            pulse /= 3.f;

            const auto f = [&](CCObject* obj_) {
                auto obj = castToGameObject(obj_);
                if (obj != nullptr && obj->m_unk32C)
                    obj->setRScale(pulse);
            };
            CCARRAY_FOREACH_B(m_pEditorLayer->m_pBatchNodeAddBottom->getChildren(), obj_)
                f(obj_);
            CCARRAY_FOREACH_B(m_pEditorLayer->m_pBatchNodeBottom2->getChildren(), obj_)
                f(obj_);
            CCARRAY_FOREACH_B(m_pEditorLayer->m_pBatchNodeBottom3->getChildren(), obj_)
                f(obj_);
            CCARRAY_FOREACH_B(m_pEditorLayer->m_pBatchNodeBottom4->getChildren(), obj_)
                f(obj_);
            CCARRAY_FOREACH_B(m_pEditorLayer->m_pBatchNodeBottom->getChildren(), obj_)
                f(obj_);
            CCARRAY_FOREACH_B(m_pEditorLayer->m_pBatchNodeAddBottom2->getChildren(), obj_)
                f(obj_);
            CCARRAY_FOREACH_B(m_pEditorLayer->m_pBatchNodeAddBottom3->getChildren(), obj_)
                f(obj_);
            CCARRAY_FOREACH_B(m_pEditorLayer->m_pBatchNodeAddBottom4->getChildren(), obj_)
                f(obj_);
            CCARRAY_FOREACH_B(m_pEditorLayer->m_pBatchNode->getChildren(), obj_)
                f(obj_);
            CCARRAY_FOREACH_B(m_pEditorLayer->m_pBatchNodeAddTop2->getChildren(), obj_)
                f(obj_);
            CCARRAY_FOREACH_B(m_pEditorLayer->m_pBatchNodeAddTop3->getChildren(), obj_)
                f(obj_);
            CCARRAY_FOREACH_B(m_pEditorLayer->m_pBatchNodeTop2->getChildren(), obj_)
                f(obj_);
            CCARRAY_FOREACH_B(m_pEditorLayer->m_pBatchNodeTop3->getChildren(), obj_)
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
    makeVisibilityPatches();
    setupRotateSaws();

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

    CCMenuItemSpriteExtra* toggleBtn;
    self->m_pSwipeBtn->getParent()->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(
                CCMenuItemSpriteExtra::create(
                    CCNodeConstructor()
                        .fromBESprite("BE_eye-on-btn.png")
                        .scale(.6f)
                        .exec([](auto t) -> void {
                            t->setContentSize(t->getScaledContentSize());
                        })
                        .done(),
                    self,
                    (SEL_MenuHandler)&EditorUI_CB::onToggleShowUI
                )
            )
            .exec([self](auto p) -> void {
                addKeybindIndicator(self, p, "betteredit.toggle_ui");
            })
            .tag(TOGGLEUI_TAG)
            .move(getShowButtonPosition(self))
            .save(&toggleBtn)
            .done()
    );
    updateToggleButtonSprite(toggleBtn);

    loadEditorLayerInput(self);
    setupGroupFilterButton(self);
    loadGridButtons(self);
    loadSliderPercent(self);
    loadClipboard(self);
    loadPasteRepeatButton(self);
    loadAutoSaveTimer(self);
    loadVisibilityTab(self);
    loadEditorKeybindIndicators(self);

    if (BetterEdit::isEditorViewOnlyMode()) {
        auto viewOnlyLabel = CCLabelBMFont::create("View-Only Mode", "bigFont.fnt");

        viewOnlyLabel->setScale(.4f);
        viewOnlyLabel->setOpacity(90);
        viewOnlyLabel->setPosition(winSize.width / 2, winSize.height - 30.0f);

        self->addChild(viewOnlyLabel);

        auto backButton = CCMenuItemSpriteExtra::create(
            CCNodeConstructor()
                .fromFrameName("GJ_arrow_01_001.png")
                .scale(.75f)
                .done(),
            self,
            menu_selector(EditorUI_CB::onExitViewMode)
        );

        backButton->setPosition(
            self->m_pPlaybackBtn->getPositionX(),
            self->m_pTrashBtn->getPositionY()
        );
        backButton->setTag(VIEWMODE_BACKBTN_TAG);

        self->m_pPlaybackBtn->getParent()->addChild(backButton);

        self->showUI(false);
    }
    
    BetterEdit::sharedState()->m_bHookConflictFound = false;

    g_hasResetObjectsScale = true;
    self->schedule(schedule_selector(EditorUIPulse::updateObjectsPulse));

    return true;
}

GDMAKE_HOOK(0x7a370)
CCArray* __fastcall EditorUI_createCustomItems(EditorUI* self) {
    setIgnoreNewObjectsForSliderPercent(true);

    auto ret = GDMAKE_ORIG_P(self);

    setIgnoreNewObjectsForSliderPercent(false);

    return ret;
}

GDMAKE_HOOK(0x7a280)
void __fastcall EditorUI_onDeleteCustomItem(EditorUI* self, edx_t edx, CCObject* pSender) {
    setIgnoreNewObjectsForSliderPercent(true);

    GDMAKE_ORIG_V(self, edx, pSender);

    setIgnoreNewObjectsForSliderPercent(false);
}

GDMAKE_HOOK(0x79fd0)
void __fastcall EditorUI_onNewCustomItem(EditorUI* self, edx_t edx, CCObject* pSender) {
    setIgnoreNewObjectsForSliderPercent(true);

    GDMAKE_ORIG_V(self, edx, pSender);

    setIgnoreNewObjectsForSliderPercent(false);
}

GDMAKE_HOOK(0x8def0)
void __fastcall EditorUI_transformObjectCall(EditorUI* self, edx_t edx, CCObject* pSender) {
    GDMAKE_ORIG_V(self, edx, pSender);

    SoftSaveManager::save();
}

GDMAKE_HOOK(0x78280)
void __fastcall EditorUI_updateButtons(EditorUI* self) {
    GDMAKE_ORIG_V(self);

    updatePasteRepeatButton(self);
}

GDMAKE_HOOK(0x87180)
void __fastcall EditorUI_showUI(gd::EditorUI* self, edx_t edx, bool show) {
    if (BetterEdit::isEditorViewOnlyMode())
        show = false;
    
    if (!g_showUI) {
        show = false;
    }

    GDMAKE_ORIG_V(self, edx, show);

    g_uiIsVisible = show;

    if (BetterEdit::isEditorViewOnlyMode()) {
        CCARRAY_FOREACH_B_TYPE(
            self->m_pPlaybackBtn->getParent()->getChildren(),
            node,
            CCNode
        ) {
            node->setVisible(node->getTag() == VIEWMODE_BACKBTN_TAG);
        }
    }

    auto toggleBtn = as<CCMenuItemSpriteExtra*>(
        self->m_pSwipeBtn->getParent()->getChildByTag(TOGGLEUI_TAG)
    );

    self->m_pTabsMenu->setVisible(self->m_nSelectedMode == 2 && show);
    CATCH_NULL(self->m_pCopyBtn->getParent()->getChildByTag(7777))->setVisible(show);
    showGridButtons(self, show);
    showLayerControls(self, show);
    showVisibilityTab(self, show);

    updateEditorKeybindIndicators();

    if (toggleBtn) {
        toggleBtn->setVisible(show || !g_showUI);
        self->m_pPlaytestBtn->setVisible(g_showUI);
        if (show)
            updateToggleButtonSprite(toggleBtn);
    }
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
void __fastcall EditorUI_keyDown(EditorUI* self_, edx_t, enumKeyCodes key) {
    auto self = offset_cast<EditorUI*>(self_, -0xf8);
    
    KeybindManager::get()->executeEditorCallbacks(
        Keybind(key), self, true, BetterEdit::isEditorViewOnlyMode()
    );
}

GDMAKE_HOOK(0x92180)
void __fastcall EditorUI_keyUp(EditorUI* self_, edx_t, enumKeyCodes key) {
    auto self = offset_cast<EditorUI*>(self_, -0xf8);

    KeybindManager::get()->executeEditorCallbacks(
        Keybind(key), self, false, BetterEdit::isEditorViewOnlyMode()
    );
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

