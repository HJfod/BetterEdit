#include "loadEditorKeybindIndicators.hpp"
#include "KeybindNode.hpp"

void addIndicator(CCNode* target, const char* keybind) {
    auto node = KeybindNode::create(keybind);

    node->setPosition({
        target->getContentSize().width / 2,
        0.0f
    });

    target->addChild(node);
}

void clearEditorKeybindIndicators(EditorUI*) {
    KeybindNode::clearList();
}

void loadEditorKeybindIndicators(EditorUI* ui) {
    addIndicator(ui->m_pBuildModeBtn, "Build Mode");
    addIndicator(ui->m_pEditModeBtn, "Edit Mode");
    addIndicator(ui->m_pDeleteModeBtn, "Delete Mode");
    addIndicator(ui->m_pSwipeBtn, "Swipe");
    addIndicator(ui->m_pFreeMoveBtn, "Free Move");
    addIndicator(ui->m_pDeselectBtn, "Deselect All");
    addIndicator(ui->m_pSnapBtn, "Snap");
    addIndicator(ui->m_pRotateBtn, "Rotate");
    addIndicator(ui->m_pPlaybackBtn, "Playback Music");
    addIndicator(ui->m_pPlaytestBtn, "Playtest");
    addIndicator(ui->m_pPlaytestStopBtn, "Playtest");
    addIndicator(ui->m_pTrashBtn, "Delete Selected");
    addIndicator(ui->m_pLinkBtn, "Link Objects");
    addIndicator(ui->m_pUnlinkBtn, "Unlink Objects");
    addIndicator(ui->m_pUndoBtn, "Undo");
    addIndicator(ui->m_pRedoBtn, "Redo");
    addIndicator(ui->m_pEditObjectBtn, "Edit Object");
    addIndicator(ui->m_pEditGroupBtn, "Edit Group");
    addIndicator(ui->m_pEditHSVBtn, "Toggle HSV Picker");
    addIndicator(ui->m_pEditSpecialBtn, "Edit Special");
    addIndicator(ui->m_pCopyPasteBtn, "Duplicate");
    addIndicator(ui->m_pCopyBtn, "Copy");
    addIndicator(ui->m_pPasteBtn, "Paste");
    addIndicator(ui->m_pCopyValuesBtn, "Copy Values");
    addIndicator(ui->m_pPasteStateBtn, "Paste State");
    addIndicator(ui->m_pPasteColorBtn, "Paste Color");
    addIndicator(ui->m_pGoToLayerBtn, "Go To Layer");
    addIndicator(ui->m_pGuideToggle, "Toggle Guide");
    addIndicator(ui->m_pLayerNextBtn, "Next Group");
    addIndicator(ui->m_pLayerPrevBtn, "Previous Group");
    addIndicator(ui->m_pGoToBaseBtn, "Go To Base Layer");
}
