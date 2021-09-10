#include <GDMake.h>
#include "passTouch.hpp"
#include "../tools/CustomKeybinds/loadEditorKeybindIndicators.hpp"

using namespace gdmake;

// thank you cos8o
// adapted from https://github.com/cos8oih/editorFreezeFix

GDMAKE_HOOK(0x87600)
void __fastcall EditorUI_onPlayTest(gd::EditorUI* self, edx_t edx, cocos2d::CCObject* pSender) {
    if (
        BetterEdit::getPulseObjectsInEditor() &&
        !BetterEdit::getHasShownPulseVolumeWarning() &&
        !FMODAudioEngine::sharedEngine()->m_fBackgroundMusicVolume 
    ) {
        FLAlertLayer::create(
            nullptr,
            "Warning",
            "OK", nullptr,
            320.0f,
            "<cy>Pulsing objects</c> do not work when you have "
            "volume turned down! (You can turn up volume in "
            "<cg>BetterEdit settings</c>)"
        )->show();
        BetterEdit::setHasShownPulseVolumeWarning(true);
        return;
    }

    self->m_bTouchDown = false;
    if (!g_bHoldingDownTouch)
        GDMAKE_ORIG_V(self, edx, pSender);
}
