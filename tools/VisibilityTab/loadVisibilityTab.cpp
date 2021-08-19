#include "loadVisibilityTab.hpp"
#include "../../hooks/EditorPauseLayer.hpp"

// for patching stuff
static constexpr const float f_96 = 96.0f;
static constexpr const float f_0  = 0.0f;

static constexpr const int VIEWBUTTONBAR_TAG = 0x234592;

bool g_bHideLDM = false;

#define UPD_IMG(_var_, _tag_, _tvar_) \
    if (_var_) \
    as<ButtonSprite*>(as<CCMenuItemSpriteExtra*>(_var_)->getNormalImage())  \
        ->updateBGImage(_tvar_ == _tag_ ? "GJ_button_02.png" : "GJ_button_01.png");

class VisibilityToggleCallbacks : public CCObject {
    public:
        void onToggle(CCObject* pSender) {
            auto data = as<bool*>(as<CCNode*>(pSender)->getUserData());

            *data = !as<CCMenuItemToggler*>(pSender)->isToggled();
        }
};

ButtonSprite* createBtnSprite(const char* file) {
    auto spr = ButtonSprite::create(
        createBESprite(file),
        0x32, true, .6f, 0, "GJ_button_01.png", true, 0x32
    );

    return spr;
}

CCMenuItemToggler* createEditBtn(
    EditorUI* target,
    const char* file,
    SEL_MenuHandler cb = nullptr,
    bool* b = nullptr
) {
    auto sprOff = ButtonSprite::create(
        createBESprite(file),
        0x32, true, 1.0f, 0, "GJ_button_01.png", true, 0x32
    );
    sprOff->m_pSubSprite->setScale(1.25f);

    auto sprOn = ButtonSprite::create(
        createBESprite(file),
        0x32, true, 1.0f, 0, "GJ_button_02.png", true, 0x32
    );
    sprOn->m_pSubSprite->setScale(1.25f);

    if (b) cb = (SEL_MenuHandler)&VisibilityToggleCallbacks::onToggle;

    auto btn = CCMenuItemToggler::create(
        sprOff, sprOn, target, cb
    );

    if (b) btn->setUserData(as<void*>(b));

    return btn;
}

void makeVisibilityPatches() {
    patch(0x768ed, intToBytes(as<int>(&f_96)), true);
    patch(0x8c9f8, intToBytes(as<int>(&f_0)), true);
    patch(0x7af6b, intToBytes(as<int>(&f_0)), true);
    patch(0x7cc7b, intToBytes(as<int>(&f_0)), true);
}

void loadVisibilityTab(EditorUI* self) {
    self->m_pBuildModeBtn->setNormalImage(createBtnSprite("BE_create_01.png"));
    self->m_pBuildModeBtn->setContentSize(
        self->m_pBuildModeBtn->getNormalImage()->getScaledContentSize()
    );
    self->m_pBuildModeBtn->setPosition(-260, -93.5f);

    self->m_pEditModeBtn->setNormalImage(createBtnSprite("BE_edit_01.png"));
    self->m_pEditModeBtn->setContentSize(
        self->m_pEditModeBtn->getNormalImage()->getScaledContentSize()
    );
    self->m_pEditModeBtn->setPosition(-216.5f, -93.5f);

    self->m_pDeleteModeBtn->setNormalImage(createBtnSprite("BE_delete_01.png"));
    self->m_pDeleteModeBtn->setContentSize(
        self->m_pDeleteModeBtn->getNormalImage()->getScaledContentSize()
    );
    self->m_pDeleteModeBtn->setPosition(-260, -136.5f);

    auto vTabBtn = CCMenuItemSpriteExtra::create(
        createBtnSprite("BE_visible_01.png"),
        self,
        (SEL_MenuHandler)&EditorUI::toggleMode
    );

    vTabBtn->setTag(4);
    vTabBtn->setPosition(-216.5f, -136.5f);

    self->m_pBuildModeBtn->getParent()->addChild(vTabBtn);

    UPD_IMG(self->m_pBuildModeBtn, 2, self->m_nSelectedMode);
    UPD_IMG(self->m_pDeleteModeBtn, 1, self->m_nSelectedMode);
    UPD_IMG(self->m_pEditModeBtn, 3, self->m_nSelectedMode);
    UPD_IMG(self->m_pBuildModeBtn->getParent()->getChildByTag(4), 4, self->m_nSelectedMode);



    auto btns = CCArray::create();

    btns->addObject(createEditBtn(
        self, "BE_v_rotate.png", (SEL_MenuHandler)&EditorPauseLayer_CB::onRotateSaws
    ));
    btns->addObject(createEditBtn(
        self, "BE_v_ldm.png", nullptr, &g_bHideLDM
    ));
    btns->addObject(createEditBtn(
        self, "BE_v_pulse.png"
    ));
    btns->addObject(createEditBtn(
        self, "BE_v_prevmode.png"
    ));

    auto buttonBar = EditButtonBar::create(
        btns,
        { CCDirector::sharedDirector()->getWinSize().width / 2, 86.0f },
        self->m_pTabsArray->count(), false,
        GameManager::sharedState()->getIntGameVariable("0049"),
        GameManager::sharedState()->getIntGameVariable("0050")
    );
    buttonBar->setTag(VIEWBUTTONBAR_TAG);
    buttonBar->setVisible(self->m_nSelectedMode == 4);

    self->addChild(buttonBar, 10);
}

void updateVisibilityTab(EditorUI* self) {
    as<EditButtonBar*>(self->getChildByTag(VIEWBUTTONBAR_TAG))
        ->reloadItemsInNormalSize();
}

bool shouldHideLDMObject(GameObject* obj) {
    return g_bHideLDM && obj->m_bHighDetail;
}

GDMAKE_HOOK(0x7ad20)
void __fastcall EditorUI_toggleMode(EditorUI* self, edx_t edx, CCObject* pSender) {
    if (!self->m_pBuildModeBtn->getParent()->getChildByTag(4))
        return GDMAKE_ORIG_V(self, edx, pSender);

    auto tag = pSender->getTag();

    if (self->m_nSelectedMode != tag) {
        self->m_nSelectedMode = tag;

        UPD_IMG(self->m_pBuildModeBtn, 2, tag);
        UPD_IMG(self->m_pDeleteModeBtn, 1, tag);
        UPD_IMG(self->m_pEditModeBtn, 3, tag);
        UPD_IMG(self->m_pBuildModeBtn->getParent()->getChildByTag(4), 4, tag);

        // manually doing EditorUI::resetUI
        // because that replaces the sprites
        // and we don't want that

        self->m_pDeleteMenu->setVisible(tag == 1);
        self->updateDeleteMenu();
        self->updateCreateMenu(false);
        self->m_pEditButtonBar->setVisible(tag == 3);
        self->updateGridNodeSize();
        self->getChildByTag(VIEWBUTTONBAR_TAG)->setVisible(tag == 4);
    }
}
