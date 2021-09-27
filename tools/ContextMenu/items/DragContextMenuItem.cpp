#include "../ContextMenu.hpp"
#include "../CustomizeCMLayer.hpp"
#include "../../EditorLayerInput/editorLayerInput.hpp"

#define MORD(x, d) (this->m_pMenu ? this->m_pMenu->x : d)

bool DragContextMenuItem::init(
    ContextMenu* menu,
    const char* spr,
    const char* txt,
    DragContextMenuItem::DragCallback cb,
    DragContextMenuItem::DragValue cv
) {
    if (!SpecialContextMenuItem::init(menu, spr, txt, nullptr))
        return false;

    this->m_pDragCallback = cb;
    this->m_pDragValue = cv;
    this->m_sText = txt;

    return true;
}

void DragContextMenuItem::visit() {
    if (this->m_pDragValue) {
        this->m_pLabel->setString(
            (
                this->m_sText +
                ": "_s +
                BetterEdit::formatToString(this->m_pDragValue(), 2u)
            ).c_str() 
        );
    }

    SpecialContextMenuItem::visit();
}

void DragContextMenuItem::drag(float val) {
    if (this->m_pDragCallback)
        this->m_pDragCallback(this, val);
}

DragContextMenuItem* DragContextMenuItem::create(
    ContextMenu* menu,
    const char* spr,
    const char* txt,
    DragContextMenuItem::DragCallback cb,
    DragContextMenuItem::DragValue cv
) {
    auto ret = new DragContextMenuItem;

    if (ret && ret->init(menu, spr, txt, cb, cv)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

