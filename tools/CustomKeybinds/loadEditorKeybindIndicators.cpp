#include "loadEditorKeybindIndicators.hpp"
#include "KeybindNode.hpp"
#include "../../hooks/EditorPauseLayer.hpp"

void addKeybindIndicator(EditorUI* ui, CCNode* target, const char* keybind) {
    auto k = KeybindNode::create(target, keybind);
    ui->addChild(k);
    k->setVisible(shouldShowKeybindIndicator());
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
    addKeybindIndicator(ui, ui->m_pBuildModeBtn, "Build Mode");
    addKeybindIndicator(ui, ui->m_pEditModeBtn, "Edit Mode");
    addKeybindIndicator(ui, ui->m_pDeleteModeBtn, "Delete Mode");
    addKeybindIndicator(ui, ui->m_pSwipeBtn, "Swipe");
    addKeybindIndicator(ui, ui->m_pFreeMoveBtn, "Free Move");
    addKeybindIndicator(ui, ui->m_pDeselectBtn, "Deselect");
    addKeybindIndicator(ui, ui->m_pSnapBtn, "Snap");
    addKeybindIndicator(ui, ui->m_pRotateBtn, "Rotate");
    addKeybindIndicator(ui, ui->m_pPlaybackBtn, "Playback Music");
    addKeybindIndicator(ui, ui->m_pPlaytestBtn, "Playtest");
    addKeybindIndicator(ui, ui->m_pPlaytestStopBtn, "Playtest");
    addKeybindIndicator(ui, ui->m_pTrashBtn, "Delete Selected");
    addKeybindIndicator(ui, ui->m_pLinkBtn, "Link Objects");
    addKeybindIndicator(ui, ui->m_pUnlinkBtn, "Unlink Objects");
    addKeybindIndicator(ui, ui->m_pUndoBtn, "Undo");
    addKeybindIndicator(ui, ui->m_pRedoBtn, "Redo");
    addKeybindIndicator(ui, ui->m_pEditObjectBtn, "Edit Object");
    addKeybindIndicator(ui, ui->m_pEditGroupBtn, "Edit Group");
    addKeybindIndicator(ui, ui->m_pEditHSVBtn, "Toggle HSV Picker");
    addKeybindIndicator(ui, ui->m_pEditSpecialBtn, "Edit Special");
    addKeybindIndicator(ui, ui->m_pCopyPasteBtn, "Duplicate");
    addKeybindIndicator(ui, ui->m_pCopyBtn, "Copy");
    addKeybindIndicator(ui, ui->m_pPasteBtn, "Paste");
    addKeybindIndicator(ui, ui->m_pCopyValuesBtn, "Copy Values");
    addKeybindIndicator(ui, ui->m_pPasteStateBtn, "Paste State");
    addKeybindIndicator(ui, ui->m_pPasteColorBtn, "Paste Color");
    addKeybindIndicator(ui, ui->m_pGoToLayerBtn, "Go To Layer");
    addKeybindIndicator(ui, ui->m_pGoToBaseBtn, "Go To Base Layer");
    addKeybindIndicator(ui, ui->m_pGuideToggle, "Toggle Guide");
    addKeybindIndicator(ui, ui->m_pLayerNextBtn, "Next Group");
    addKeybindIndicator(ui, ui->m_pLayerPrevBtn, "Previous Group");
    addKeybindIndicator(ui, getChild<CCNode*>(ui->m_pPlaybackBtn->getParent(), 13), "Zoom In");
    addKeybindIndicator(ui, getChild<CCNode*>(ui->m_pPlaybackBtn->getParent(), 14), "Zoom Out");
    addKeybindIndicator(ui, getChild<CCNode*>(ui->m_pLayerPrevBtn->getParent(), 0), "Pause");
    addKeybindIndicator(ui, getChild<CCNode*>(ui->m_pLayerPrevBtn->getParent(), 1), "Open Level Settings");
    
    auto ix = 0;
    for (auto editKeybind : {
        "Object Up Small",
        "Object Down Small",
        "Object Left Small",
        "Object Right Small",
        "Object Up",
        "Object Down",
        "Object Left",
        "Object Right",
        "Flip X",
        "Flip Y",
        "Rotate CW",
        "Rotate CCW",
        "Rotate 45 CW",
        "Rotate 45 CCW",
        "Object Up Big",
        "Object Down Big",
        "Object Left Big",
        "Object Right Big",
        "Object Up Tiny",
        "Object Down Tiny",
        "Object Left Tiny",
        "Object Right Tiny",
        "Rotate",
        "Rotate Snap",
        "Show Scale Control",
    }) {
        addKeybindIndicator(
            ui,
            as<CCNode*>(ui->m_pEditButtonBar->m_pButtonArray->objectAtIndex(ix++)),
            editKeybind
        );
    }

    updateEditorKeybindIndicators();
}

void showEditorKeybindIndicatorIfItsTargetIsBeingHovered() {
    if (!BetterEdit::getShowKeybindOnHover()) return;
    if (shouldShowKeybindIndicator()) return;

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
