#include "loadEditorKeybindIndicators.hpp"
#include "KeybindNode.hpp"
#include "../../hooks/EditorPauseLayer.hpp"

void addIndicator(EditorUI* ui, CCNode* target, const char* keybind) {
    ui->addChild(KeybindNode::create(target, keybind));
}

void clearEditorKeybindIndicators(EditorUI*) {
    KeybindNode::clearList();
}

void updateEditorKeybindIndicators() {
    KeybindNode::updateList();
}

void showEditorKeybindIndicators(bool show) {
    KeybindNode::showList(show);
}

void loadEditorKeybindIndicators(EditorUI* ui) {
    addIndicator(ui, ui->m_pBuildModeBtn, "Build Mode");
    addIndicator(ui, ui->m_pEditModeBtn, "Edit Mode");
    addIndicator(ui, ui->m_pDeleteModeBtn, "Delete Mode");
    addIndicator(ui, ui->m_pSwipeBtn, "Swipe");
    addIndicator(ui, ui->m_pFreeMoveBtn, "Free Move");
    addIndicator(ui, ui->m_pDeselectBtn, "Deselect");
    addIndicator(ui, ui->m_pSnapBtn, "Snap");
    addIndicator(ui, ui->m_pRotateBtn, "Rotate");
    addIndicator(ui, ui->m_pPlaybackBtn, "Playback Music");
    addIndicator(ui, ui->m_pPlaytestBtn, "Playtest");
    addIndicator(ui, ui->m_pPlaytestStopBtn, "Playtest");
    addIndicator(ui, ui->m_pTrashBtn, "Delete Selected");
    addIndicator(ui, ui->m_pLinkBtn, "Link Objects");
    addIndicator(ui, ui->m_pUnlinkBtn, "Unlink Objects");
    addIndicator(ui, ui->m_pUndoBtn, "Undo");
    addIndicator(ui, ui->m_pRedoBtn, "Redo");
    addIndicator(ui, ui->m_pEditObjectBtn, "Edit Object");
    addIndicator(ui, ui->m_pEditGroupBtn, "Edit Group");
    addIndicator(ui, ui->m_pEditHSVBtn, "Toggle HSV Picker");
    addIndicator(ui, ui->m_pEditSpecialBtn, "Edit Special");
    addIndicator(ui, ui->m_pCopyPasteBtn, "Duplicate");
    addIndicator(ui, ui->m_pCopyBtn, "Copy");
    addIndicator(ui, ui->m_pPasteBtn, "Paste");
    addIndicator(ui, ui->m_pCopyValuesBtn, "Copy Values");
    addIndicator(ui, ui->m_pPasteStateBtn, "Paste State");
    addIndicator(ui, ui->m_pPasteColorBtn, "Paste Color");
    addIndicator(ui, ui->m_pGoToLayerBtn, "Go To Layer");
    addIndicator(ui, ui->m_pGuideToggle, "Toggle Guide");
    addIndicator(ui, ui->m_pLayerNextBtn, "Next Group");
    addIndicator(ui, ui->m_pLayerPrevBtn, "Previous Group");
    addIndicator(ui, ui->m_pGoToBaseBtn, "Go To Base Layer");

    showEditorKeybindIndicators(shouldShowKeybindIndicator());
}

void showEditorKeybindIndicatorIfItsTargetIsBeingHovered() {
    if (!BetterEdit::getShowKeybindOnHover()) return;

    CCARRAY_FOREACH_B_TYPE(KeybindNode::getList(), node, KeybindNode) {
        auto csize = node->getTarget()->getScaledContentSize();
        auto pos = node->getTarget()->getPosition();
        if (node->getTarget()->getParent())
            pos = node->getTarget()->getParent()->convertToWorldSpace(pos);
        
        auto rect = CCRect {
            pos.x - csize.width / 2,
            pos.y - csize.height / 2,
            csize.width,
            csize.height
        };

        node->setVisible(rect.containsPoint(getMousePos()));
    }
}
