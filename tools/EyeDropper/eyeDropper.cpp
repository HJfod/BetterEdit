#include "eyeDropper.hpp"
#include "EyeDropperColorOverlay.hpp"

static constexpr const int DROPPER_TAG = 0xb00b;
bool g_bPickingColor = false;
ccColor3B g_obColorUnderCursor;
EyeDropperColorOverlay* g_pOverlay;
HWND g_hwnd;

void updateEyeDropperColor() {
    if (g_bPickingColor) {
        POINT mpos;
        HDC hdc = GetWindowDC(GetDesktopWindow());

        GetCursorPos(&mpos);

        auto col = GetPixel(hdc, mpos.x, mpos.y);

        g_obColorUnderCursor.r = GetRValue(col);
        g_obColorUnderCursor.g = GetGValue(col);
        g_obColorUnderCursor.b = GetBValue(col);

        if (g_pOverlay) {
            g_pOverlay->setShowColor(g_obColorUnderCursor);
            g_pOverlay->setLabelText(WindowFromPoint(mpos) == g_hwnd);
        }
    }
}

bool handleEyeDropperClick(int btn, int pressed) {
    POINT mpos;
    GetCursorPos(&mpos);

    auto w = WindowFromPoint(mpos);

    if (g_bPickingColor && (btn == 1 || w != g_hwnd)) {
        g_bPickingColor = false;

        g_pOverlay->finish(&g_obColorUnderCursor);

        ReleaseCapture();
        SetFocus(g_hwnd);
        BringWindowToTop(g_hwnd);
        return true;
    }

    return false;
}

class PickCallback : public CCObject {
    public:
        void onPickColor(CCObject* pSender) {
            if (g_bPickingColor) {
                SetCapture(g_hwnd);
                return;
            }

            auto popup = as<ColorSelectPopup*>(as<CCNode*>(pSender)->getUserData());

            g_bPickingColor = true;
            g_pOverlay = EyeDropperColorOverlay::addToCurrentScene(popup);
            g_pOverlay->setStatusBool(&g_bPickingColor);

            g_hwnd = getGDHWND();

            SetCapture(g_hwnd);
        }
};

CCMenuItemSpriteExtra* createEyeDropperButton(CCNode* target) {
    auto spr = ButtonSprite::create(
        createBESprite("BE_eyedropper.png"),
        0, 0, 1.0f, 0, "GJ_button_05.png", true, 0
    );

    spr->setScale(.75f);

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
    if (popup->isColorTrigger)
        btn->setPosition(110, 163);
    else
        btn->setPosition(-194, 146);

    popup->m_pButtonMenu->addChild(btn);

    showEyeDropper(popup);
}

void loadEyeDropper(SetupPulsePopup* popup) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto btn = createEyeDropperButton(popup);

    btn->setUserData(popup);
    btn->setPosition(-169, 215);

    popup->m_pButtonMenu->addChild(btn);

    showEyeDropper(popup);
}

bool isPickingEyeDropperColor() {
    return g_bPickingColor;
}
