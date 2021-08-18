#include "loadVisibilityTab.hpp"

// for patching stuff
static constexpr const float f_96 = 96.0f;
static constexpr const float f_0  = 0.0f;

#define UPD_IMG(_var_, _tag_) \
    if (_var_) \
    as<ButtonSprite*>(as<CCMenuItemSpriteExtra*>(_var_)->getNormalImage())  \
        ->updateBGImage(tag == _tag_ ? "GJ_button_02.png" : "GJ_button_01.png");

ButtonSprite* createBtnSprite(const char* file) {
    auto spr = ButtonSprite::create(
        createBESprite(file),
        0x32, true, .8f, 0, "GJ_button_01.png", true, 0x32
    );

    return spr;
}

ButtonSprite* createBtnSpriteT(const char* text) {
    auto spr = ButtonSprite::create(
        CCLabelBMFont::create(text, "bigFont.fnt"),
        0x32, true, 1.0f, 0, "GJ_button_01.png", true, 0x32
    );

    return spr;
}

void makeVisibilityPatches() {
    patch(0x768ed, intToBytes(as<int>(&f_96)), true);
    patch(0x8c9f8, intToBytes(as<int>(&f_0)), true);
    patch(0x7af6b, intToBytes(as<int>(&f_0)), true);
    patch(0x7cc7b, intToBytes(as<int>(&f_0)), true);
}

void loadVisibilityTab(EditorUI* self) {
    self->m_pBuildModeBtn->setNormalImage(createBtnSpriteT("+"));
    self->m_pBuildModeBtn->setContentSize(
        self->m_pBuildModeBtn->getNormalImage()->getScaledContentSize()
    );
    self->m_pBuildModeBtn->setPosition(-260, -93.5f);

    self->m_pEditModeBtn->setNormalImage(createBtnSpriteT("E"));
    self->m_pEditModeBtn->setContentSize(
        self->m_pEditModeBtn->getNormalImage()->getScaledContentSize()
    );
    self->m_pEditModeBtn->setPosition(-216.5f, -93.5f);

    self->m_pDeleteModeBtn->setNormalImage(createBtnSprite("edit_delBtn_001.png"));
    self->m_pDeleteModeBtn->setContentSize(
        self->m_pDeleteModeBtn->getNormalImage()->getScaledContentSize()
    );
    self->m_pDeleteModeBtn->setPosition(-260, -136.5f);

    auto vTabBtn = CCMenuItemSpriteExtra::create(
        createBtnSprite("BE_eye-on.png"),
        self,
        (SEL_MenuHandler)&EditorUI::toggleMode
    );

    vTabBtn->setTag(4);
    vTabBtn->setPosition(-216.5f, -136.5f);

    self->m_pBuildModeBtn->getParent()->addChild(vTabBtn);

    self->toggleMode(self->m_pBuildModeBtn->getParent()->getChildByTag(
        self->m_nSelectedMode
    ));
}

GDMAKE_HOOK(0x7ad20)
void __fastcall EditorUI_toggleMode(EditorUI* self, edx_t edx, CCObject* pSender) {
    if (!self->m_pBuildModeBtn->getParent()->getChildByTag(4))
        return GDMAKE_ORIG_V(self, edx, pSender);

    auto tag = pSender->getTag();

    if (self->m_nSelectedMode != tag) {
        self->m_nSelectedMode = tag;

        UPD_IMG(self->m_pBuildModeBtn, 2);
        UPD_IMG(self->m_pDeleteModeBtn, 1);
        UPD_IMG(self->m_pEditModeBtn, 3);
        UPD_IMG(self->m_pBuildModeBtn->getParent()->getChildByTag(4), 4);

        // manually doing EditorUI::resetUI
        // because that replaces the sprites
        // and we don't want that

        self->m_pDeleteMenu->setVisible(tag == 1);
        self->updateDeleteMenu();
        self->updateCreateMenu(false);
        self->m_pEditButtonBar->setVisible(tag == 3);
        self->updateGridNodeSize();
    }
}
