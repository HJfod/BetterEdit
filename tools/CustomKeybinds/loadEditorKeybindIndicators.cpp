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
    addKeybindIndicator(ui, ui->m_pBuildModeBtn, "gd.edit.build_mode");
    addKeybindIndicator(ui, ui->m_pEditModeBtn, "gd.edit.edit_mode");
    addKeybindIndicator(ui, ui->m_pDeleteModeBtn, "gd.edit.delete_mode");
    addKeybindIndicator(ui, ui->m_pSwipeBtn, "gd.edit.toggle_swipe");
    addKeybindIndicator(ui, ui->m_pFreeMoveBtn, "gd.edit.toggle_free_move");
    addKeybindIndicator(ui, ui->m_pDeselectBtn, "gd.edit.deselect");
    addKeybindIndicator(ui, ui->m_pSnapBtn, "gd.edit.toggle_snap");
    addKeybindIndicator(ui, ui->m_pRotateBtn, "gd.edit.toggle_rotate");
    addKeybindIndicator(ui, ui->m_pPlaybackBtn, "gd.edit.playback_music");
    addKeybindIndicator(ui, ui->m_pPlaytestBtn, "gd.edit.playtest");
    addKeybindIndicator(ui, ui->m_pPlaytestStopBtn, "gd.edit.playtest");
    addKeybindIndicator(ui, ui->m_pTrashBtn, "gd.edit.delete_selected");
    addKeybindIndicator(ui, ui->m_pLinkBtn, "betteredit.link_objects");
    addKeybindIndicator(ui, ui->m_pUnlinkBtn, "betteredit.unlink_objects");
    addKeybindIndicator(ui, ui->m_pUndoBtn, "gd.edit.undo");
    addKeybindIndicator(ui, ui->m_pRedoBtn, "gd.edit.redo");
    addKeybindIndicator(ui, ui->m_pEditObjectBtn, "betteredit.edit_object");
    addKeybindIndicator(ui, ui->m_pEditGroupBtn, "betteredit.edit_group");
    addKeybindIndicator(ui, ui->m_pEditHSVBtn, "betteredit.toggle_hsv_picker");
    addKeybindIndicator(ui, ui->m_pEditSpecialBtn, "betteredit.edit_special");
    addKeybindIndicator(ui, ui->m_pCopyPasteBtn, "gd.edit.copy_and_paste");
    addKeybindIndicator(ui, ui->m_pCopyBtn, "gd.edit.copy");
    addKeybindIndicator(ui, ui->m_pPasteBtn, "gd.edit.paste");
    addKeybindIndicator(ui, ui->m_pCopyValuesBtn, "betteredit.copy_values");
    addKeybindIndicator(ui, ui->m_pPasteStateBtn, "betteredit.paste_state");
    addKeybindIndicator(ui, ui->m_pPasteColorBtn, "betteredit.paste_color");
    addKeybindIndicator(ui, ui->m_pGoToLayerBtn, "betteredit.go_to_layer");
    addKeybindIndicator(ui, ui->m_pGoToBaseBtn, "betteredit.go_to_base_layer");
    // addKeybindIndicator(ui, ui->m_pGuideToggle, "Toggle Guide");
    addKeybindIndicator(ui, ui->m_pLayerNextBtn, "gd.edit.next_layer");
    addKeybindIndicator(ui, ui->m_pLayerPrevBtn, "gd.edit.prev_layer");
    addKeybindIndicator(ui, getChild<CCNode*>(ui->m_pPlaybackBtn->getParent(), 13), "gd.edit.zoom_in");
    addKeybindIndicator(ui, getChild<CCNode*>(ui->m_pPlaybackBtn->getParent(), 14), "gd.edit.zoom_out");
    addKeybindIndicator(ui, getChild<CCNode*>(ui->m_pLayerPrevBtn->getParent(), 0), "gd.play.pause");
    addKeybindIndicator(ui, getChild<CCNode*>(ui->m_pLayerPrevBtn->getParent(), 1), "betteredit.open_level_settings");
    
    auto ix = 0;
    for (auto editKeybind : {
        "gd.edit.move_obj_small_up",
        "gd.edit.move_obj_small_down",
        "gd.edit.move_obj_small_left",
        "gd.edit.move_obj_small_right",
        "gd.edit.move_obj_up",
        "gd.edit.move_obj_down",
        "gd.edit.move_obj_left",
        "gd.edit.move_obj_right",
        "gd.edit.flip_x",
        "gd.edit.flip_y",
        "gd.edit.rotate_cw",
        "gd.edit.rotate_ccw",
        "betteredit.rotate_45_cw",
        "betteredit.rotate_45_ccw",
        "gd.edit.move_obj_big_up",
        "gd.edit.move_obj_big_down",
        "gd.edit.move_obj_big_left",
        "gd.edit.move_obj_big_right",
        "gd.edit.move_obj_tiny_up",
        "gd.edit.move_obj_tiny_down",
        "gd.edit.move_obj_tiny_left",
        "gd.edit.move_obj_tiny_right",
        "gd.edit.toggle_rotate",
        "betteredit.rotate_snap",
        "betteredit.show_scale_control",
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
