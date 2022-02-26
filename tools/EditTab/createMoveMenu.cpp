#include "moveForCommand.hpp"
#include "../EditorLayerInput/editorLayerInput.hpp"
#include "CustomEditMenu.hpp"

void addMoveButton(
    EditorUI* self,
    const char* spr,
    const char* sizeTxt,
    EditCommand com,
    const char* keybind = nullptr,
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

GDMAKE_HOOK(0x8c0d0, "_ZN8EditorUI14createMoveMenuEv")
void __fastcall EditorUI_createMoveMenu(EditorUI* self) {
    if (BetterEdit::getEnableCustomEditMenu()) {
        self->m_pEditButtonDict = CCDictionary::create();
        self->m_pEditButtonDict->retain();
        self->m_pEditButtonBar = CustomEditMenu::create(self);
        self->addChild(self->m_pEditButtonBar);
        return;
    }

    GDMAKE_ORIG_V(self);

    addMoveButton(self, "edit_upBtn2_001.png",      "1/2",  kEditCommandHalfUp,
        "betteredit.move_obj_half_up");
    addMoveButton(self, "edit_downBtn2_001.png",    "1/2",  kEditCommandHalfDown,
        "betteredit.move_obj_half_down");
    addMoveButton(self, "edit_leftBtn2_001.png",    "1/2",  kEditCommandHalfLeft,
        "betteredit.move_obj_half_left");
    addMoveButton(self, "edit_rightBtn2_001.png",   "1/2",  kEditCommandHalfRight,
        "betteredit.move_obj_half_right");
    
    addMoveButton(self, "edit_upBtn2_001.png",      "1/4",  kEditCommandQuarterUp,
        "betteredit.move_obj_quarter_up");
    addMoveButton(self, "edit_downBtn2_001.png",    "1/4",  kEditCommandQuarterDown,
        "betteredit.move_obj_quarter_down");
    addMoveButton(self, "edit_leftBtn2_001.png",    "1/4",  kEditCommandQuarterLeft,
        "betteredit.move_obj_quarter_left");
    addMoveButton(self, "edit_rightBtn2_001.png",   "1/4",  kEditCommandQuarterRight,
        "betteredit.move_obj_quarter_right");
    
    addMoveButton(self, "edit_upBtn2_001.png",      "1/8",  kEditCommandEigthUp,
        "betteredit.move_obj_eigth_up");
    addMoveButton(self, "edit_downBtn2_001.png",    "1/8",  kEditCommandEigthDown,
        "betteredit.move_obj_eigth_down");
    addMoveButton(self, "edit_leftBtn2_001.png",    "1/8",  kEditCommandEigthLeft,
        "betteredit.move_obj_eigth_left");
    addMoveButton(self, "edit_rightBtn2_001.png",   "1/8",  kEditCommandEigthRight,
        "betteredit.move_obj_eigth_right");
    
    addMoveButton(self, "edit_upBtn_001.png",       "1/4",  kEditCommandQUnitUp,    
        "betteredit.move_obj_unit_up", .8f);
    addMoveButton(self, "edit_downBtn_001.png",     "1/4",  kEditCommandQUnitDown,  
        "betteredit.move_obj_unit_down", .8f);
    addMoveButton(self, "edit_leftBtn_001.png",     "1/4",  kEditCommandQUnitLeft,  
        "betteredit.move_obj_unit_left", .8f);
    addMoveButton(self, "edit_rightBtn_001.png",    "1/4",  kEditCommandQUnitRight, 
        "betteredit.move_obj_unit_right", .8f);

    self->m_pEditButtonBar->reloadItemsInNormalSize();
}
