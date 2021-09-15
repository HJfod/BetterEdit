#include "BEKeybinds.hpp"
#include "../AutoSave/autoSave.hpp"
#include "../../hooks/EditorUI.hpp"
#include "../GroupIDFilter/AdvancedFilterLayer.hpp"
#include "../GridSize/gridButton.hpp"
#include "../../hooks/EditorPauseLayer.hpp"
#include "../RotateSaws/rotateSaws.hpp"
#include "../VisibilityTab/loadVisibilityTab.hpp"
#include "../EditorLayerInput/editorLayerInput.hpp"
#include "../HalfMove/moveForCommand.hpp"
#include "../RepeatPaste/repeatPaste.hpp"
#include "../History/UndoHistoryPopup.hpp"

static constexpr const float w_edge = 120.0f;

enum Direction {
    kDirLeft, kDirRight, kDirUp, kDirDown,
};

void moveGameLayerSmoothAbs(EditorUI* ui, CCPoint const& pos) {
    ui->m_pEditorLayer->getObjectLayer()->stopActionByTag(0xbb);
    auto a = CCEaseInOut::create(
        CCMoveTo::create(.4f, pos),
        2.0f
    );
    a->setTag(0xbb);
    ui->m_pEditorLayer->getObjectLayer()->runAction(a);
}

void moveGameLayerSmooth(EditorUI* ui, CCPoint const& pos) {
    auto opos = ui->m_pEditorLayer->getObjectLayer()->getPosition();
    auto npos = opos + pos;
    moveGameLayerSmoothAbs(ui, npos);
}

void focusGameLayerOnObject(EditorUI* ui, GameObject* obj) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto pos = ui->m_pEditorLayer->getObjectLayer()
        ->convertToWorldSpace(obj->getPosition());
    
    auto rpos = pos - winSize;

    moveGameLayerSmooth(ui, rpos);
}

void focusGameLayerToSelection(EditorUI* ui) {
    CCPoint pos;

    if (ui->m_pSelectedObject)
        pos = ui->m_pSelectedObject->getPosition();
    else if (ui->m_pSelectedObjects)
        pos = ui->getGroupCenter(ui->m_pSelectedObjects, false);
    else return;

    auto olayer = ui->m_pEditorLayer->getObjectLayer();
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto gpos = olayer->convertToWorldSpace(pos);
    auto npos = (-pos + winSize / 2) * olayer->getScale();
    auto opos = olayer->getPosition();

    auto mx = (
        gpos.x < w_edge ||
        gpos.x > winSize.width - w_edge
    );
    auto my = (
        gpos.y < w_edge ||
        gpos.y > winSize.height - w_edge
    );

    if (mx && my)
        moveGameLayerSmoothAbs(ui, { npos.x, npos.y });
    else if (mx)
        moveGameLayerSmoothAbs(ui, { npos.x, opos.y });
    else if (my)
        moveGameLayerSmoothAbs(ui, { opos.x, npos.y });
}

CCArray* g_pSelectedObjects = nullptr;
Direction g_eLastDirection;

bool isSelectable(GameObject* obj) {
    if (obj->m_bIsSelected)
        return false;

    auto l = LevelEditorLayer::get()->m_nCurrentLayer;
    if (l == -1)
        return true;
    
    if (obj->m_nEditorLayer == l)
        return true;

    if (obj->m_nEditorLayer2 == l)
        return true;
    
    return false;
}

void selectObjectDirection(EditorUI* ui, Direction dir) {
    CCPoint startPos = CCPointZero;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    if (ui->m_pSelectedObject)
        startPos = ui->m_pSelectedObject->getPosition();
    else if (ui->m_pSelectedObjects)
        startPos = ui->getGroupCenter(ui->m_pSelectedObjects, false);
    else
        startPos = ui->m_pEditorLayer->getObjectLayer()->convertToNodeSpace(
            winSize / 2
        );

    static constexpr const float range = 200.0f;
    static constexpr const float too_far = 500.0f;
    static constexpr const float too_near = 15.0f;

    GameObject* nearest = nullptr;

    CCARRAY_FOREACH_B_TYPE(ui->m_pEditorLayer->getAllObjects(), obj, GameObject) {
        switch (dir) {
            case kDirLeft: case kDirRight:
                if (
                    obj->getPositionY() > startPos.y - range &&
                    obj->getPositionY() < startPos.y + range
                ) {
                    if (!isSelectable(obj))
                        continue;

                    if (dir == kDirLeft) {
                        if (obj->getPositionX() > startPos.x - too_near)
                            continue;
                        if (obj->getPositionX() < startPos.x - too_far)
                            continue;

                        if (!nearest) {
                            nearest = obj;
                            continue;
                        }

                        if (nearest->getPositionX() < obj->getPositionX())
                            nearest = obj;
                    } else {
                        if (obj->getPositionX() < startPos.x + too_near)
                            continue;
                        if (obj->getPositionX() > startPos.x + too_far)
                            continue;

                        if (!nearest) {
                            nearest = obj;
                            continue;
                        }

                        if (nearest->getPositionX() > obj->getPositionX())
                            nearest = obj;
                    }
                }
                break;

            case kDirUp: case kDirDown:
                if (
                    obj->getPositionX() > startPos.x - range &&
                    obj->getPositionX() < startPos.x + range
                ) {
                    if (!isSelectable(obj))
                        continue;

                    if (dir == kDirUp) {
                        if (obj->getPositionY() < startPos.y + too_near)
                            continue;
                        if (obj->getPositionY() > startPos.y + too_far)
                            continue;

                        if (!nearest) {
                            nearest = obj;
                            continue;
                        }

                        if (nearest->getPositionY() > obj->getPositionY())
                            nearest = obj;
                    } else {
                        if (obj->getPositionY() > startPos.y - too_near)
                            continue;
                        if (obj->getPositionY() < startPos.y - too_far)
                            continue;

                        if (!nearest) {
                            nearest = obj;
                            continue;
                        }

                        if (nearest->getPositionY() < obj->getPositionY())
                            nearest = obj;
                    }
                }
                break;
        }
    }

    if (nearest) {
        if (g_eLastDirection != dir) {
            g_pSelectedObjects->removeAllObjects();
        }
        if (g_pSelectedObjects->indexOfObject(nearest) != UINT_MAX)
            return;
        
        g_pSelectedObjects->addObject(nearest);
        g_eLastDirection = dir;

        ui->selectObject(nearest, true);
        ui->updateButtons();

        auto gpos = ui->m_pEditorLayer->getObjectLayer()->convertToWorldSpace(
            nearest->getPosition()
        );

        if (
            gpos.x < w_edge ||
            gpos.x > winSize.width - w_edge ||
            gpos.y < w_edge ||
            gpos.y > winSize.height - w_edge
        )
            moveGameLayerSmooth(ui, { -gpos.x + winSize.width / 2, -gpos.y + winSize.height / 2 });
    }
}

void loadBEKeybinds() {
    g_pSelectedObjects = CCArray::create();
    g_pSelectedObjects->retain();

    auto kbm = KeybindManager::get();

    kbm->addEditorKeybind({ "Object Left Half", "betteredit.move_obj_half_left",
        [](EditorUI* ui) -> bool {
            if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
                ui->moveObjectCall(kEditCommandHalfLeft);
            return false;
        }, "editor.move"
    }, {});

    kbm->addEditorKeybind({ "Object Right Half", "betteredit.move_obj_half_right",
        [](EditorUI* ui) -> bool {
            if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
                ui->moveObjectCall(kEditCommandHalfRight);
            return false;
        }, "editor.move"
    }, {});

    kbm->addEditorKeybind({ "Object Up Half", "betteredit.move_obj_half_up",
        [](EditorUI* ui) -> bool {
            if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
                ui->moveObjectCall(kEditCommandHalfUp);
            return false;
        }, "editor.move"
    }, {});

    kbm->addEditorKeybind({ "Object Down Half", "betteredit.move_obj_half_down",
        [](EditorUI* ui) -> bool {
            if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
                ui->moveObjectCall(kEditCommandHalfDown);
            return false;
        }, "editor.move"
    }, {});

    kbm->addEditorKeybind({ "Object Left Quarter", "betteredit.move_obj_quarter_left",
        [](EditorUI* ui) -> bool {
            if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
                ui->moveObjectCall(kEditCommandQuarterLeft);
            return false;
        }, "editor.move"
    }, {});

    kbm->addEditorKeybind({ "Object Right Quarter", "betteredit.move_obj_quarter_right",
        [](EditorUI* ui) -> bool {
            if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
                ui->moveObjectCall(kEditCommandQuarterRight);
            return false;
        }, "editor.move"
    }, {});

    kbm->addEditorKeybind({ "Object Up Quarter", "betteredit.move_obj_quarter_up",
        [](EditorUI* ui) -> bool {
            if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
                ui->moveObjectCall(kEditCommandQuarterUp);
            return false;
        }, "editor.move"
    }, {});

    kbm->addEditorKeybind({ "Object Down Quarter", "betteredit.move_obj_quarter_down",
        [](EditorUI* ui) -> bool {
            if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
                ui->moveObjectCall(kEditCommandQuarterDown);
            return false;
        }, "editor.move"
    }, {});

    kbm->addEditorKeybind({ "Rotate 45 CCW", "betteredit.rotate_45_ccw",
        [](EditorUI* ui) -> bool {
            ui->transformObjectCall(kEditCommandRotateCCW45);
            return false;
        }, "editor.modify"
    }, {});

    kbm->addEditorKeybind({ "Rotate 45 CW", "betteredit.rotate_45_cw",
        [](EditorUI* ui) -> bool {
            ui->transformObjectCall(kEditCommandRotateCW45);
            return false;
        }, "editor.modify"
    }, {});

    kbm->addEditorKeybind({ "Rotate Snap", "betteredit.rotate_snap",
        [](EditorUI* ui) -> bool {
            ui->transformObjectCall(kEditCommandRotateSnap);
            return false;
        }, "editor.modify", false
    }, {});

    kbm->addEditorKeybind({ "Show Scale Control", "betteredit.show_scale_control",
        [](EditorUI* ui) -> bool {
            ui->activateScaleControl(nullptr);
            return false;
        }, "editor.ui", false
    }, {});

    kbm->addEditorKeybind({ "Cancel Autosave", "betteredit.cancel_autosave",
        [](EditorUI* ui) -> bool {
            if (getAutoSaveTimer(ui)->cancellable()) {
                getAutoSaveTimer(ui)->cancel();
                return true;
            }
            return false;
        }, "editor.global", false
    }, {{ KEY_Escape, 0 }});

    kbm->addEditorKeybind({ "Scroll Zoom Modifier", "betteredit.zoom_modifier",
        true, "editor.global"
    }, {{ KEY_None, Keybind::kmControl }});

    kbm->addEditorKeybind({ "Scroll Horizontal Modifier", "betteredit.horizontal_modifier",
        true, "editor.global"
    }, {{ KEY_None, Keybind::kmShift }});

    kbm->addEditorKeybind({ "Save Level", "betteredit.save_level",
        [](EditorUI* ui) -> bool {
            auto saveLabel = CCLabelBMFont::create("Saving...", "bigFont.fnt");
            saveLabel->setPosition(CCDirector::sharedDirector()->getWinSize() / 2);
            saveLabel->setScale(.6f);
            ui->addChild(saveLabel, 999);
            ui->visit();
            ui->draw();
            // > Don't call it manually
            CCDirector::sharedDirector()->drawScene();

            auto pause = EditorPauseLayer::create(ui->m_pEditorLayer);
            pause->saveLevel();
            pause->release();

            saveLabel->removeFromParent();
            saveLabel->release();
            
            return false;
        }, "editor.global", false
    }, {});

    kbm->addEditorKeybind({ "Select All", "betteredit.select_all",
        [](EditorUI* ui) -> bool {
            if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
                ui->selectAll();
            return false;
        }, "editor.select", false
    }, {});

    kbm->addEditorKeybind({ "Select All Left", "betteredit.select_all_left",
        [](EditorUI* ui) -> bool {
            if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
                ui->selectAllWithDirection(false);
            return false;
        }, "editor.select", false
    }, {});

    kbm->addEditorKeybind({ "Select All Right", "betteredit.select_all_right",
        [](EditorUI* ui) -> bool {
            if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
                ui->selectAllWithDirection(true);
            return false;
        }, "editor.select", false
    }, {});

    kbm->addEditorKeybind({ "Select Object Left", "betteredit.select_obj_left",
        [](EditorUI* ui) -> bool {
            selectObjectDirection(ui, kDirLeft);
            return false;
        }, "editor.select"
    }, {});

    kbm->addEditorKeybind({ "Select Object Right", "betteredit.select_obj_right",
        [](EditorUI* ui) -> bool {
            selectObjectDirection(ui, kDirRight);
            return false;
        }, "editor.select"
    }, {});

    kbm->addEditorKeybind({ "Select Object Up", "betteredit.select_obj_up",
        [](EditorUI* ui) -> bool {
            selectObjectDirection(ui, kDirUp);
            return false;
        }, "editor.select"
    }, {});

    kbm->addEditorKeybind({ "Select Object Down", "betteredit.select_obj_down",
        [](EditorUI* ui) -> bool {
            selectObjectDirection(ui, kDirDown);
            return false;
        }, "editor.select"
    }, {});

    kbm->addEditorKeybind({ "Toggle UI", "betteredit.toggle_ui",
        [](EditorUI* ui) -> bool {
            toggleShowUI(ui);
            return false;
        }, "editor.ui", false
    }, {});

    kbm->addEditorKeybind({ "Show UI", "betteredit.show_ui",
        [](EditorUI* ui) -> bool {
            ui->showUI(true);
            return false;
        }, "editor.ui", false
    }, {});

    kbm->addEditorKeybind({ "Hide UI", "betteredit.hide_ui",
        [](EditorUI* ui) -> bool {
            ui->showUI(false);
            return false;
        }, "editor.ui", false
    }, {});

    kbm->addEditorKeybind({ "Preview Mode", "betteredit.preview_mode",
        [](EditorUI* ui) -> bool {
            auto b = GameManager::sharedState()->getGameVariable("0036");
            GameManager::sharedState()->setGameVariable("0036", !b);
            ui->m_pEditorLayer->updateEditorMode();
            updateVisibilityTab(ui);
            return false;
        }, "editor.visibility", false
    }, {});

    kbm->addEditorKeybind({ "Edit Object", "betteredit.edit_object",
        [](EditorUI* ui) -> bool {
            ui->editObject(nullptr);
            return false;
        }, "editor.modify", false
    }, {});

    kbm->addEditorKeybind({ "Edit Group", "betteredit.edit_group",
        [](EditorUI* ui) -> bool {
            ui->editGroup(nullptr);
            return false;
        }, "editor.modify", false
    }, {});

    kbm->addEditorKeybind({ "Edit Special", "betteredit.edit_special",
        [](EditorUI* ui) -> bool {
            ui->editObject2(nullptr);
            return false;
        }, "editor.modify", false
    }, {});

    // kbm->addEditorKeybind({ "Repeat Copy + Paste", "betteredit.repeat_paste",
    //     [](EditorUI* ui) -> bool {
    //         repeatPaste(ui);
    //         return false;
    //     }, "editor.modify", false
    // }, {});

    kbm->addEditorKeybind({ "Open Level Settings", "betteredit.open_level_settings",
        [](EditorUI* ui) -> bool {
            ui->onSettings(nullptr);
            return false;
        }, "editor.global", false
    }, {});

    kbm->addEditorKeybind({ "Build Helper", "betteredit.build_helper",
        [](EditorUI* ui) -> bool {
            ui->dynamicGroupUpdate(false);
            return false;
        }, "editor.special", false
    }, {});

    kbm->addEditorKeybind({ "Create Base", "betteredit.create_base",
        [](EditorUI* ui) -> bool {
            ui->createRockBase();
            return false;
        }, "editor.special", false
    }, {});

    kbm->addEditorKeybind({ "Create Edges", "betteredit.create_edges",
        [](EditorUI* ui) -> bool {
            ui->createRockEdges();
            return false;
        }, "editor.special", false
    }, {});

    kbm->addEditorKeybind({ "Create Outline", "betteredit.create_outline",
        [](EditorUI* ui) -> bool {
            ui->createRockOutline();
            return false;
        }, "editor.special", false
    }, {});

    kbm->addEditorKeybind({ "Uncheck All", "betteredit.uncheck_all_portals",
        [](EditorUI* ui) -> bool {
            auto p = EditorPauseLayer::create(ui->m_pEditorLayer);
            p->uncheckAllPortals(nullptr);
            p->release();

            return false;
        }, "editor.global", false
    }, {});

    kbm->addEditorKeybind({ "Copy Values", "betteredit.copy_values",
        [](EditorUI* ui) -> bool {
            ui->onCopyState(nullptr);
            return false;
        }, "editor.modify", false
    }, {});

    kbm->addEditorKeybind({ "Paste State", "betteredit.paste_state",
        [](EditorUI* ui) -> bool {
            ui->onPasteState(nullptr);
            return false;
        }, "editor.modify", false
    }, {});

    kbm->addEditorKeybind({ "Paste Color", "betteredit.paste_color",
        [](EditorUI* ui) -> bool {
            ui->onPasteColor(nullptr);
            return false;
        }, "editor.modify", false
    }, {});

    kbm->addEditorKeybind({ "Toggle HSV Picker", "betteredit.toggle_hsv_picker",
        [](EditorUI* ui) -> bool {
            ui->editColor(nullptr);
            return false;
        }, "editor.ui", false
    }, {});

    kbm->addEditorKeybind({ "Link Objects", "betteredit.link_objects",
        [](EditorUI* ui) -> bool {
            ui->onGroupSticky(nullptr);
            return false;
        }, "editor.modify", false
    }, {});

    kbm->addEditorKeybind({ "Unlink Objects", "betteredit.unlink_objects",
        [](EditorUI* ui) -> bool {
            ui->onUngroupSticky(nullptr);
            return false;
        }, "editor.modify", false
    }, {});

    kbm->addEditorKeybind({ "Go To Layer", "betteredit.go_to_layer",
        [](EditorUI* ui) -> bool {
            ui->onGoToLayer(nullptr);
            return false;
        }, "editor.ui", false
    }, {});

    kbm->addEditorKeybind({ "Go To Next Free Layer", "betteredit.go_to_next_free_layer",
        [](EditorUI* ui) -> bool {
            as<EditorUI_CB*>(ui)->onNextFreeEditorLayer(nullptr);
            return false;
        }, "editor.ui", false
    }, {});

    kbm->addEditorKeybind({ "Go To Base Layer", "betteredit.go_to_base_layer",
        [](EditorUI* ui) -> bool {
            ui->onGoToBaseLayer(nullptr);
            return false;
        }, "editor.ui", false
    }, {});

    kbm->addEditorKeybind({ "Open Advanced Filter", "betteredit.open_advanced_filter",
        [](EditorUI* ui) -> bool {
            AdvancedFilterLayer::create()->show();
            return false;
        }, "editor.modify", false
    }, {});

    kbm->addEditorKeybind({ "Increase Grid Size", "betteredit.increase_grid_size",
        [](EditorUI* ui) -> bool {
            zoomEditorGrid(ui, true);
            return false;
        }, "editor.global", false
    }, {});

    kbm->addEditorKeybind({ "Decrease Grid Size", "betteredit.decrease_grid_size",
        [](EditorUI* ui) -> bool {
            zoomEditorGrid(ui, false);
            return false;
        }, "editor.global", false
    }, {});

    kbm->addEditorKeybind({ "Toggle Grid", "betteredit.toggle_grid",
        [](EditorUI* ui) -> bool {
            GameManager::sharedState()->toggleGameVariable("0038");
            ui->m_pEditorLayer->updateOptions();
            updateVisibilityTab(ui);
            return false;
        }, "editor.visibility", false
    }, {});

    kbm->addEditorKeybind({ "Rotate Saws", "betteredit.rotate_saws",
        [](EditorUI* ui) -> bool {
            enableRotations(!shouldRotateSaw());
            if (shouldRotateSaw())
                beginRotations(LevelEditorLayer::get());
            else
                stopRotations(LevelEditorLayer::get());
            updateVisibilityTab(ui);
            return false;
        }, "editor.visibility", false
    }, {});

    kbm->addEditorKeybind({ "Toggle LDM", "betteredit.toggle_ldm",
        [](EditorUI* ui) -> bool {
            setHideLDMObjects();
            updateVisibilityTab(ui);
            return false;
        }, "editor.visibility", false
    }, {});

    kbm->addEditorKeybind({ "Lock Layer", "betteredit.lock_layer",
        [](EditorUI* ui) -> bool {
            as<EditorUI_CB*>(ui)->onLockLayer(nullptr);
            return false;
        }, "editor.ui", false
    }, {});

    kbm->addEditorKeybind({ "View Layers", "betteredit.view_layer_list",
        [](EditorUI* ui) -> bool {
            as<EditorUI_CB*>(ui)->onShowLayerPopup(nullptr);
            return false;
        }, "editor.ui", false
    }, {});

    kbm->addEditorKeybind({ "Align X", "betteredit.align_x",
        [](EditorUI* ui) -> bool {
            ui->alignObjects(ui->getSelectedObjects(), false);
            return false;
        }, "editor.modify", false
    }, {});

    kbm->addEditorKeybind({ "Align Y", "betteredit.align_y",
        [](EditorUI* ui) -> bool {
            ui->alignObjects(ui->getSelectedObjects(), true);
            return false;
        }, "editor.modify", false
    }, {});

    kbm->addEditorKeybind({ "View Undo History", "betteredit.view_undo_history",
        [](EditorUI* ui) -> bool {
            UndoHistoryPopup::create()->show();
            return false;
        }, "editor.modify", false
    }, {});
}
