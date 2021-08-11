#include <GDMake.h>
#include "../BetterEdit.hpp"
#include "../tools/settings/BESettingsLayer.hpp"
#include "../tools/LiveCollab/pauseMenuHook.hpp"
#include "../tools/AutoColorTriggers/autoCT.hpp"
#include "../tools/LevelPercent/levelPercent.hpp"
#include "../tools/IDRemap/remapHook.hpp"
#include "../tools/PasteString/loadPasteButton.hpp"
#include "../tools/RotateSaws/rotateSaws.hpp"
#include "../tools/EyeDropper/eyeDropper.hpp"
#include "../tools/AutoSave/autoSave.hpp"

using namespace gdmake;

bool g_bRotateSaws = false;

bool shouldRotateSaw() { return g_bRotateSaws; }

class EditorPauseLayer_CB : public gd::EditorPauseLayer {
    public:
        void onBESettings(cocos2d::CCObject* pSender) {
            BESettingsLayer::create(this)->show();
        }

        void onRotateSaws(CCObject* pSender) {
            g_bRotateSaws = !as<CCMenuItemToggler*>(pSender)->isToggled();
        }
};

GDMAKE_HOOK(0x758d0)
void __fastcall EditorPauseLayer_keyDown(EditorPauseLayer* self, edx_t edx, enumKeyCodes key) {
    if (key == KEY_Escape)
        as<EditorPauseLayer*>(as<uintptr_t>(self) - 0xf8)->onResume(nullptr);
    else
        GDMAKE_ORIG_V(self, edx, key);
}

GDMAKE_HOOK(0x74fe0)
void __fastcall EditorPauseLayer_onResume(EditorPauseLayer* self, edx_t edx, CCObject* pSender) {
    GDMAKE_ORIG_V(self, edx, pSender);

    for (auto const& addr : std::initializer_list<int> {
        0x73169,
        0x856A4, 
        0x87B17,
        0x87BC7,
        0x87D95,
        0x880F4,
        0x160B06,
    })
        if (BetterEdit::getBypassObjectLimit())
            patch(addr, { 0xff, 0xff, 0xff, 0x7f });
        else unpatch(addr);
    
    if (BetterEdit::getBypassObjectLimit()) {
        patch(0x7A100, { 0xeb });
        patch(0x7A022, { 0xeb });
        patch(0x7A203, { 0x90, 0x90 });
    } else {
        unpatch(0x7A100);
        unpatch(0x7A022);
        unpatch(0x7A203);
    }

    if (BetterEdit::getUseUpArrowForGameplay()) {
        patch(0x91abb, { 0x3d, 0x26, 0x00 });
        patch(0x921a6, { 0x3d, 0x26, 0x00 });
    } else {
        unpatch(0x91abb);
        unpatch(0x921a6);
    }

    if (g_bRotateSaws)
        beginRotations(self->m_pEditorLayer);
    else
        stopRotations(self->m_pEditorLayer);

    updatePercentLabelPosition(LevelEditorLayer::get()->getEditorUI());
    // showPositionLabel(LevelEditorLayer::get()->getEditorUI(), true);
}

GDMAKE_HOOK(0x75660)
void __fastcall EditorPauseLayer_onExitEditor(EditorPauseLayer* self, edx_t edx, CCObject* pSender) {
    stopRotations(self->m_pEditorLayer);
    resetAutoSaveTimer(self->m_pEditorLayer->m_pEditorUI);

    GDMAKE_ORIG_V(self, edx, pSender);

    self->removeFromParentAndCleanup(true);
}

GDMAKE_HOOK(0x730e0)
bool __fastcall EditorPauseLayer_init(EditorPauseLayer* self, edx_t edx, LevelEditorLayer* el) {
    if (!GDMAKE_ORIG(self, edx, el))
        return false;

    auto menu = as<CCMenu*>(self->m_pButton0->getParent());
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    auto gdSettingsBtn = getChild<CCMenu*>(menu, menu->getChildrenCount() - 1);

    auto btn = CCMenuItemSpriteExtra::create(
        CCNodeConstructor()
            .fromFrameName("GJ_optionsBtn02_001.png")
            .scale(.8f)
            .done(),
        self,
        (SEL_MenuHandler)&EditorPauseLayer_CB::onBESettings
    );
    btn->setPosition(winSize.width / 2 - 70, gdSettingsBtn->getPositionY());
    menu->addChild(btn);

    gdSettingsBtn->setPositionX(winSize.width / 2 - 30);

    loadLiveButton(self);
    loadColorTriggerButton(self);
    loadRemapHook(self);
    loadPasteButton(self);

    GameToolbox::createToggleButton(
        (SEL_MenuHandler)&EditorPauseLayer_CB::onRotateSaws,
        g_bRotateSaws, as<CCMenu*>(self->m_pButton0->getParent()),
        self, self, .55f, .42f, 85.0f, "", false, 0, nullptr,
        "Preview Saws",
        {
            CCDirector::sharedDirector()->getScreenLeft() + 25.0f,
            CCDirector::sharedDirector()->getScreenBottom() + 192.0f
        },
        { 8.0f, 0.0f }
    );

    if (isPickingEyeDropperColor()) {
        FLAlertLayer::create(
            nullptr,
            "Warning",
            "OK", nullptr,
            "You can not leave the <co>Editor</c> with the "
            "<cl>Color Picker</c> enabled - <cr>this will cause a guaranteed crash!</c>"
        )->show();
    }

    return true;
}
