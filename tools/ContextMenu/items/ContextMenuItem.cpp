#include "../ContextMenu.hpp"
#include "../CustomizeCMLayer.hpp"
#include "../../EditorLayerInput/editorLayerInput.hpp"

#define MORD(x, d) (this->m_pMenu ? this->m_pMenu->x : d)

bool ContextMenuItem::init(ContextMenu* menu) {
    if (!CCNode::init())
        return false;
    
    this->m_pMenu = menu;
    this->updateDragDir();

    return true;
}

void ContextMenuItem::updateDragDir() {
    if (this->m_pMenu) {
        // override drag direction if needs be
        switch (this->m_pMenu->m_eDragDir) {
            case ContextMenu::kDragItemDirHorizontal:
                this->m_bHorizontalDrag = true;
                break;
            case ContextMenu::kDragItemDirVertical:
                this->m_bHorizontalDrag = false;
                break;
        }
    }
}

void ContextMenuItem::draw() {
    ccColor4B col = m_pMenu->m_colHover;

    // some mf somewhere resets the blend
    // func sometimes
    ccGLBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    if (!this->m_bNoHover) {
        if (this->m_bSuperMouseHovered) {
            if (this->m_nFade < m_pMenu->m_colHover.a)
                this->m_nFade += MORD(m_nAnimationSpeed, 5);
            else
                this->m_nFade = m_pMenu->m_colHover.a;
        } else {
            if (this->m_nFade >= MORD(m_nAnimationSpeed, 5))
                this->m_nFade -= MORD(m_nAnimationSpeed, 5);
            else
                this->m_nFade = 0;
        }

        col.a = this->m_nFade;

        ccDrawSolidRect(
            { 0, 0 },
            this->getScaledContentSize(),
            to4f(col)
        );
    }

    if (m_pMenu && m_pMenu->m_bDrawBorder) {
        ccDrawColor4B(m_pMenu->m_colBorder);
        ccDrawRect(
            { 0, 0 },
            this->getScaledContentSize()
        );
    }

    auto mul = this->m_pMenu ? this->m_pMenu->getScale() : 1.f;

    this->setSuperMouseHitSize(this->getScaledContentSize() * mul);
    this->setSuperMouseHitOffset(this->getScaledContentSize() / 2 * mul);

    CCNode::draw();
}

bool ContextMenuItem::mouseDownSuper(MouseButton btn, CCPoint const& mpos) {
    if (!this->m_bDisabled && btn == kMouseButtonLeft) {
        if (this->m_pMenu) this->m_pMenu->deactivateOthers(this);
        this->activate();
        this->m_obLastMousePos = mpos;
        SuperMouseManager::get()->captureMouse(this);
    }
    if (this->m_bDisabled) {
        this->m_obLastMousePos = mpos;
    }
    return true;
}

bool ContextMenuItem::mouseUpSuper(MouseButton btn, CCPoint const&) {
    if (!this->m_bDisabled && btn == kMouseButtonLeft) {
        SuperMouseManager::get()->releaseCapture(this);
    }
    return true;
}

void ContextMenuItem::mouseMoveSuper(CCPoint const& mpos) {
    if (!this->m_bDisabled && this->m_bSuperMouseDown) {
        if (this->m_bHorizontalDrag) {
            this->drag(mpos.x - this->m_obLastMousePos.x);
        } else {
            this->drag(mpos.y - this->m_obLastMousePos.y);
        }
        this->m_obLastMousePos = mpos;
    }
    if (this->m_bDisabled && this->m_bSuperMouseDown) {
        this->setPosition(
            this->getPosition() + (this->m_obLastMousePos - mpos) * MORD(getScale(), 1.f)
        );
    }
}

bool ContextMenuItem::mouseScrollSuper(float y, float) {
    auto val = y / -m_fScrollDivisor;
    if (MORD(m_bInvertedScrollWheel, false))
        val = -val;
    this->drag(val);
    return true;
}

AnyLabel* ContextMenuItem::createLabel(const char* txt) {
    if (this->m_pMenu) {
        switch (this->m_pMenu->m_eType) {
            case kAnyLabelTypeBM:
                return AnyLabel::create(CCLabelBMFont::create(txt, this->m_pMenu->m_sFont));
                break;
            
            case kAnyLabelTypeTTF:
                return AnyLabel::create(CCLabelTTF::create(
                    txt, this->m_pMenu->m_sFont, this->m_pMenu->m_fTTFFontSize
                ));
                break;
        }
    }
    return AnyLabel::create(CCLabelTTF::create(txt, "Segoe UI", 16));
}

void ContextMenuItem::activate() {}
void ContextMenuItem::deactivate() {}
void ContextMenuItem::updateItem() {}
void ContextMenuItem::drag(float) {}
void ContextMenuItem::hide() { this->deactivate(); }
