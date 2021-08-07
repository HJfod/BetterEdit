#include "eyeDropper.hpp"
#include "EyeDropperColorOverlay.hpp"

static constexpr const int DROPPER_TAG = 0xb00b;
bool g_bPickingColor = false;
ccColor3B g_obColorUnderCursor;
ColorSelectPopup* g_pTargetLayer;
ColorAction* g_pTargetChannel;
EyeDropperColorOverlay* g_pOverlay;
HWND g_hwnd;

HWND glfwGetWin32Window(GLFWwindow* wnd) {
    auto cocosBase = GetModuleHandleA("libcocos2d.dll");

    auto pRet = reinterpret_cast<HWND(__cdecl*)(GLFWwindow*)>(
        reinterpret_cast<uintptr_t>(cocosBase) + 0x112c10
    )(wnd);

    return pRet;
}

class PickCallback : public CCObject {
    public:
        void onPickColor(CCObject* pSender) {
            if (g_bPickingColor)
                return;

            g_pTargetLayer = as<ColorSelectPopup*>(as<CCNode*>(pSender)->getUserData());
            g_pTargetLayer->retain();
            g_pTargetChannel = g_pTargetLayer->colorAction;

            g_bPickingColor = true;

            g_pOverlay = EyeDropperColorOverlay::addToCurrentScene();

            g_hwnd = glfwGetWin32Window(CCDirector::sharedDirector()->getOpenGLView()->getWindow());

            SetCapture(g_hwnd);
        }
};

GDMAKE_HOOK("libcocos2d.dll::?onGLFWMouseCallBack@CCEGLView@cocos2d@@IAEXPAUGLFWwindow@@HHH@Z")
void __fastcall CCEGLView_onGLFWMouseCallBack(CCEGLView* self, edx_t edx, GLFWwindow* wnd, int btn, int pressed, int z) {
    if (btn == 1 && g_bPickingColor) {
        g_bPickingColor = false;

        if (g_pTargetLayer) {
            g_pTargetLayer->setPickerColor(g_obColorUnderCursor);
            g_pTargetLayer->release();
        } else if (g_pTargetChannel)
            g_pTargetChannel->m_color = g_obColorUnderCursor;
        
        g_pOverlay->removeFromParentAndCleanup(true);
        ReleaseCapture();
        SetFocus(g_hwnd);
        BringWindowToTop(g_hwnd);
        return;
    }

    return GDMAKE_ORIG_V(self, edx, wnd, btn, pressed, z);
}

GDMAKE_HOOK("libcocos2d.dll::?update@CCScheduler@cocos2d@@UAEXM@Z")
void __fastcall CCScheduler_update(CCScheduler* self, edx_t edx, float dt) {
    if (g_bPickingColor) {
        POINT mpos;
        HDC hdc = GetWindowDC(GetDesktopWindow());

        GetCursorPos(&mpos);

        auto col = GetPixel(hdc, mpos.x, mpos.y);

        g_obColorUnderCursor.r = GetRValue(col);
        g_obColorUnderCursor.g = GetGValue(col);
        g_obColorUnderCursor.b = GetBValue(col);

        if (g_pOverlay)
            g_pOverlay->setShowColor(g_obColorUnderCursor);
    }

    return GDMAKE_ORIG_V(self, edx, dt);
}

CCMenuItemSpriteExtra* createEyeDropperButton(CCNode* target) {
    auto spr = ButtonSprite::create(
        CCSprite::create("BE_eyedropper.png"),
        0, 0, 1.0f, 0, "GJ_button_05.png", true, 0
    );

    auto btn = CCMenuItemSpriteExtra::create(
        spr, target, (SEL_MenuHandler)&PickCallback::onPickColor
    );

    btn->setTag(DROPPER_TAG);

    return btn;
}

void showEyeDropper(ColorSelectPopup* target) {
    auto dropper = target->m_pButtonMenu->getChildByTag(DROPPER_TAG);

    if (dropper) dropper->setVisible(!target->copyColor);
}

void showEyeDropper(SetupPulsePopup* target) {
    auto dropper = target->m_pButtonMenu->getChildByTag(DROPPER_TAG);

    if (dropper) dropper->setVisible(!target->pulseMode);
}

void loadEyeDropper(ColorSelectPopup* popup) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto btn = createEyeDropperButton(popup);

    btn->setUserData(popup);
    btn->setPosition(-190.0f, 146.0f);

    popup->m_pButtonMenu->addChild(btn);
}

void loadEyeDropper(SetupPulsePopup* popup) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto btn = createEyeDropperButton(popup);

    btn->setUserData(popup);
    btn->setPosition(-190.0f, 146.0f);

    popup->m_pButtonMenu->addChild(btn);
}
