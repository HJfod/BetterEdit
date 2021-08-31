#include "BEKeybinds.hpp"
#include "../AutoSave/autoSave.hpp"
#include "../../hooks/EditorUI.hpp"
#include "../GroupIDFilter/AdvancedFilterLayer.hpp"

enum Direction {
    kDirLeft, kDirRight, kDirUp, kDirDown,
};

void moveGameLayerSmooth(EditorUI* ui, CCPoint const& pos) {
    auto opos = ui->m_pEditorLayer->getObjectLayer()->getPosition();
    auto npos = opos + pos;

    ui->m_pEditorLayer->getObjectLayer()->runAction(
        CCEaseInOut::create(
            CCMoveTo::create(.4f, npos),
            2.0f
        )
    );
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
    static constexpr const float w_edge = 120.0f;

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

    KeybindManager::get()->addEditorKeybind({ "Rotate 45 CW", [](EditorUI* ui) -> bool {
        ui->transformObjectCall(kEditCommandRotateCW45);
        return false;
    }}, {});

    KeybindManager::get()->addEditorKeybind({ "Rotate 45 CCW", [](EditorUI* ui) -> bool {
        ui->transformObjectCall(kEditCommandRotateCCW45);
        return false;
    }}, {});

    KeybindManager::get()->addEditorKeybind({ "Rotate Snap", [](EditorUI* ui) -> bool {
        ui->transformObjectCall(kEditCommandRotateSnap);
        return false;
    }}, {});

    KeybindManager::get()->addEditorKeybind({ "Show Rotate Control", [](EditorUI* ui) -> bool {
        ui->activateRotationControl(nullptr);
        return false;
    }, false}, {});

    KeybindManager::get()->addEditorKeybind({ "Show Scale Control", [](EditorUI* ui) -> bool {
        ui->activateScaleControl(nullptr);
        return false;
    }, false}, {});

    KeybindManager::get()->addEditorKeybind({ "Cancel Autosave", [](EditorUI* ui) -> bool {
        if (getAutoSaveTimer(ui)->cancellable()) {
            getAutoSaveTimer(ui)->cancel();
            return true;
        }
        
        return false;
    }, false}, {{ KEY_Escape, 0 }});

    KeybindManager::get()->addEditorKeybind({ "Scroll Zoom Modifier", [](EditorUI* ui) -> bool {
        return false;
    }}, {{ KEY_None, Keybind::kmControl }});

    KeybindManager::get()->addEditorKeybind({ "Scroll Horizontal Modifier", [](EditorUI* ui) -> bool {
        return false;
    }}, {{ KEY_None, Keybind::kmShift }});

    KeybindManager::get()->addEditorKeybind({ "Save Level", [](EditorUI* ui) -> bool {
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
    }, false}, {});

    KeybindManager::get()->addEditorKeybind({ "Select All", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->selectAll();
        return false;
    }, false}, {});

    KeybindManager::get()->addEditorKeybind({ "Select All Left", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->selectAllWithDirection(false);
        return false;
    }, false}, {});

    KeybindManager::get()->addEditorKeybind({ "Select All Right", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->selectAllWithDirection(true);
        return false;
    }, false}, {});

    KeybindManager::get()->addEditorKeybind({ "Select Object Left", [](EditorUI* ui) -> bool {
        selectObjectDirection(ui, kDirLeft);
        return false;
    }}, {});

    KeybindManager::get()->addEditorKeybind({ "Select Object Right", [](EditorUI* ui) -> bool {
        selectObjectDirection(ui, kDirRight);
        return false;
    }}, {});

    KeybindManager::get()->addEditorKeybind({ "Select Object Up", [](EditorUI* ui) -> bool {
        selectObjectDirection(ui, kDirUp);
        return false;
    }}, {});

    KeybindManager::get()->addEditorKeybind({ "Select Object Down", [](EditorUI* ui) -> bool {
        selectObjectDirection(ui, kDirDown);
        return false;
    }}, {});

    KeybindManager::get()->addEditorKeybind({ "Toggle UI", [](EditorUI* ui) -> bool {
        toggleShowUI(ui);
        return false;
    }}, {});

    KeybindManager::get()->addEditorKeybind({ "Show UI", [](EditorUI* ui) -> bool {
        ui->showUI(true);
        return false;
    }}, {});

    KeybindManager::get()->addEditorKeybind({ "Hide UI", [](EditorUI* ui) -> bool {
        ui->showUI(false);
        return false;
    }}, {});

    KeybindManager::get()->addEditorKeybind({ "Preview Mode", [](EditorUI* ui) -> bool {
        auto b = GameManager::sharedState()->getGameVariable("0036");

        GameManager::sharedState()->setGameVariable("0036", !b);

        ui->m_pEditorLayer->updateEditorMode();
        return false;
    }}, {});

    KeybindManager::get()->addEditorKeybind({ "Edit Object", [](EditorUI* ui) -> bool {
        ui->editObject(nullptr);
        return false;
    }}, {});

    KeybindManager::get()->addEditorKeybind({ "Edit Group", [](EditorUI* ui) -> bool {
        ui->editGroup(nullptr);
        return false;
    }}, {});

    KeybindManager::get()->addEditorKeybind({ "Edit Special", [](EditorUI* ui) -> bool {
        ui->editObject2(nullptr);
        return false;
    }}, {});

    // KeybindManager::get()->addEditorKeybind({ "Increment Hue", [](EditorUI* ui) -> bool {
    //     // todo
    //     return false;
    // }}, {});

    // KeybindManager::get()->addEditorKeybind({ "Repeat Copy + Paste", [](EditorUI* ui) -> bool {
    //     // todo
    //     return false;
    // }}, {});

    KeybindManager::get()->addEditorKeybind({ "Open Level Settings", [](EditorUI* ui) -> bool {
        ui->onSettings(nullptr);
        return false;
    }, false}, {});

    KeybindManager::get()->addEditorKeybind({ "Build Helper", [](EditorUI* ui) -> bool {
        ui->dynamicGroupUpdate(false);
        return false;
    }, false}, {});

    KeybindManager::get()->addEditorKeybind({ "Create Base", [](EditorUI* ui) -> bool {
        ui->createRockBase();
        return false;
    }, false}, {});

    KeybindManager::get()->addEditorKeybind({ "Create Edges", [](EditorUI* ui) -> bool {
        ui->createRockEdges();
        return false;
    }, false}, {});

    KeybindManager::get()->addEditorKeybind({ "Create Outline", [](EditorUI* ui) -> bool {
        ui->createRockOutline();
        return false;
    }, false}, {});

    KeybindManager::get()->addEditorKeybind({ "Uncheck All", [](EditorUI* ui) -> bool {
        auto p = EditorPauseLayer::create(ui->m_pEditorLayer);
        p->uncheckAllPortals(nullptr);
        p->release();

        return false;
    }, false}, {});

    KeybindManager::get()->addEditorKeybind({ "Copy Values", [](EditorUI* ui) -> bool {
        ui->onCopyState(nullptr);
        return false;
    }, false}, {});

    KeybindManager::get()->addEditorKeybind({ "Paste State", [](EditorUI* ui) -> bool {
        ui->onPasteState(nullptr);
        return false;
    }, false}, {});

    KeybindManager::get()->addEditorKeybind({ "Paste Color", [](EditorUI* ui) -> bool {
        ui->onPasteColor(nullptr);
        return false;
    }, false}, {});

    KeybindManager::get()->addEditorKeybind({ "Toggle HSV Picker", [](EditorUI* ui) -> bool {
        ui->editColor(nullptr);
        return false;
    }, false}, {});

    KeybindManager::get()->addEditorKeybind({ "Link Objects", [](EditorUI* ui) -> bool {
        ui->onGroupSticky(nullptr);
        return false;
    }, false}, {});

    KeybindManager::get()->addEditorKeybind({ "Unlink Objects", [](EditorUI* ui) -> bool {
        ui->onUngroupSticky(nullptr);
        return false;
    }, false}, {});

    KeybindManager::get()->addEditorKeybind({ "Go To Layer", [](EditorUI* ui) -> bool {
        ui->onGoToLayer(nullptr);
        return false;
    }, false}, {});

    KeybindManager::get()->addEditorKeybind({ "Go To Base Layer", [](EditorUI* ui) -> bool {
        ui->onGoToBaseLayer(nullptr);
        return false;
    }, false}, {});

    KeybindManager::get()->addEditorKeybind({ "Open Advanced Filter", [](EditorUI* ui) -> bool {
        AdvancedFilterLayer::create()->show();
        return false;
    }, false}, {});
}
