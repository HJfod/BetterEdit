#include "moveForCommand.hpp"
#include "../EditorLayerInput/editorLayerInput.hpp"
#include "CustomEditMenu.hpp"

void addMoveButton(
    EditorUI* self,
    const char* spr,
    const char* sizeTxt,
    EditCommand com,
    float scale = 1.0f
) {
    auto btn = self->getSpriteButton(
        spr, menu_selector(EditorUI::moveObjectCall), nullptr, .9f
    );
    btn->setTag(com);

    auto label = CCLabelBMFont::create(sizeTxt, "bigFont.fnt");
    label->setScale(.35f);
    label->setZOrder(50);
    label->setPosition(btn->getContentSize().width / 2, 11.0f);
    btn->addChild(label);

    as<ButtonSprite*>(btn->getNormalImage())
        ->m_pSubSprite->setScale(scale);

    self->m_pEditButtonBar->addButton(btn, false);
}

GDMAKE_HOOK(0x8c0d0)
void __fastcall EditorUI_createMoveMenu(EditorUI* self) {
    if (BetterEdit::getEnableCustomEditMenu()) {
        self->m_pEditButtonDict = CCDictionary::create();
        self->m_pEditButtonDict->retain();
        self->m_pEditButtonBar = CustomEditMenu::create(self);
        self->addChild(self->m_pEditButtonBar);
        return;
    }

    GDMAKE_ORIG_V(self);

    addMoveButton(self, "edit_upBtn2_001.png",      "1/2",  kEditCommandHalfUp);
    addMoveButton(self, "edit_downBtn2_001.png",    "1/2",  kEditCommandHalfDown);
    addMoveButton(self, "edit_leftBtn2_001.png",    "1/2",  kEditCommandHalfLeft);
    addMoveButton(self, "edit_rightBtn2_001.png",   "1/2",  kEditCommandHalfRight);
    
    addMoveButton(self, "edit_upBtn2_001.png",      "1/4",  kEditCommandQuarterUp);
    addMoveButton(self, "edit_downBtn2_001.png",    "1/4",  kEditCommandQuarterDown);
    addMoveButton(self, "edit_leftBtn2_001.png",    "1/4",  kEditCommandQuarterLeft);
    addMoveButton(self, "edit_rightBtn2_001.png",   "1/4",  kEditCommandQuarterRight);
    
    addMoveButton(self, "edit_upBtn2_001.png",      "1/8",  kEditCommandEigthUp);
    addMoveButton(self, "edit_downBtn2_001.png",    "1/8",  kEditCommandEigthDown);
    addMoveButton(self, "edit_leftBtn2_001.png",    "1/8",  kEditCommandEigthLeft);
    addMoveButton(self, "edit_rightBtn2_001.png",   "1/8",  kEditCommandEigthRight);
    
    addMoveButton(self, "edit_upBtn_001.png",       "1/4u", kEditCommandQUnitUp,    .8f);
    addMoveButton(self, "edit_downBtn_001.png",     "1/4u", kEditCommandQUnitDown,  .8f);
    addMoveButton(self, "edit_leftBtn_001.png",     "1/4u", kEditCommandQUnitLeft,  .8f);
    addMoveButton(self, "edit_rightBtn_001.png",    "1/4u", kEditCommandQUnitRight, .8f);

    self->m_pEditButtonBar->reloadItemsInNormalSize();
}
