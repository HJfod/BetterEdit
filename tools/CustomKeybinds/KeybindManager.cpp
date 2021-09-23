#include "KeybindManager.hpp"
#include <algorithm>
#include <functional>
#include "../VisibilityTab/loadVisibilityTab.hpp"

KeybindManager* g_manager;

keybind_id::keybind_id() { value = ""; }
keybind_id::keybind_id(const char* val) { value = val; }
keybind_id::keybind_id(std::string const& val) { value = val; }
const char* keybind_id::c_str() const { return value.c_str(); }
keybind_id keybind_id::operator=(std::string const& val) {
    value = val;
    return *this;
}
bool keybind_id::operator==(keybind_id const& other) const {
    return stringToLower(this->value) == stringToLower(other.value);
}

std::string keyToStringFixed(enumKeyCodes code) {
    switch (code) {
        case KEY_None:      return "";
        case KEY_C:         return "C";
        case KEY_Multiply:  return "Mul";
        case KEY_Divide:    return "Div";
        case KEY_OEMPlus:   return "Plus";
        case KEY_OEMMinus:  return "Minus";
        
        case static_cast<enumKeyCodes>(-1):
            return "Unk";
        
        default:
            return cocos2d::CCDirector::sharedDirector()
                ->getKeyboardDispatcher()
                ->keyToString(code);
    }
}

std::string mouseToString(MouseButton btn) {
    switch (btn) {
        case kMouseButtonNone: return "";
        case kMouseButtonLeft: return "Left Click";
        case kMouseButtonDoubleClick: return "Double Click";
        case kMouseButtonRight: return "Right Click";
        case kMouseButtonMiddle: return "Middle Click";
        case kMouseButtonNext: return "Mouse Forward";
        case kMouseButtonPrev: return "Mouse Back";
        case kMouseButtonScrollUp: return "Scroll Up";
        case kMouseButtonScrollDown: return "Scroll Down";
    }
    return "Mouse Unk";
}

static DS_Dictionary* copyDict(DS_Dictionary* other) {
    auto res = new DS_Dictionary;
    if (!res) return nullptr;

    res->doc.reset(other->doc);
    res->dictTree = other->dictTree;
    res->compatible = other->compatible;

    return res;
}

bool Keybind::operator==(Keybind const& other) const {
    return
        other.key == this->key &&
        other.modifiers == this->modifiers &&
        other.mouse == this->mouse;
}

bool Keybind::operator<(Keybind const& other) const {
    return
        this->key < other.key ||
        this->modifiers < other.modifiers ||
        this->mouse << other.mouse;
}

std::string Keybind::toString() const {
    std::string res = "";

    if (this->modifiers & kmControl)    res += "Ctrl + ";
    if (this->modifiers & kmAlt)        res += "Alt + ";
    if (this->modifiers & kmShift)      res += "Shift + ";

    std::string r = "";
    if (this->mouse != kMouseButtonNone) {
        r = mouseToString(this->mouse);
    } else {
        r = keyToStringFixed(this->key);
    }

    if (r.size())
        res += r;
    else
        res = res.substr(0, res.size() - 3);
        
    return res;
}

void Keybind::save(DS_Dictionary* dict) const {
    dict->setIntegerForKey("key", this->key);
    dict->setIntegerForKey("modifiers", this->modifiers);
    dict->setIntegerForKey("click", this->mouse);
}

Keybind::Keybind() {
    this->key = KEY_None;
    this->modifiers = 0;
}

Keybind::Keybind(enumKeyCodes pressed) {
    switch (pressed) {
        case KEY_Control:   this->key = KEY_None; break;
        case KEY_Shift:     this->key = KEY_None; break;
        case KEY_Alt:       this->key = KEY_None; break;
        default:            this->key = pressed;  break;
    }

    auto kb = CCDirector::sharedDirector()->getKeyboardDispatcher();

    this->modifiers = 0;
    if (kb->getControlKeyPressed())
        this->modifiers |= this->kmControl;
    if (kb->getShiftKeyPressed())
        this->modifiers |= this->kmShift;
    if (kb->getAltKeyPressed())
        this->modifiers |= this->kmAlt;
}

Keybind::Keybind(enumKeyCodes key, Modifiers mods) {
    this->key = key;
    this->modifiers = mods;
}

Keybind::Keybind(enumKeyCodes key, int mods) {
    this->key = key;
    this->modifiers = static_cast<Modifiers>(mods);
}

Keybind::Keybind(MouseButton btn) {
    this->mouse = btn;

    auto kb = CCDirector::sharedDirector()->getKeyboardDispatcher();

    this->modifiers = 0;
    if (kb->getControlKeyPressed())
        this->modifiers |= this->kmControl;
    if (kb->getShiftKeyPressed())
        this->modifiers |= this->kmShift;
    if (kb->getAltKeyPressed())
        this->modifiers |= this->kmAlt;
}

Keybind::Keybind(MouseButton btn, Modifiers mods) {
    this->mouse = btn;
    this->modifiers = mods;
}

Keybind::Keybind(MouseButton btn, int mods) {
    this->mouse = btn;
    this->modifiers = static_cast<Modifiers>(mods);
}

Keybind::Keybind(DS_Dictionary* dict, int version) {
    this->key = static_cast<enumKeyCodes>(dict->getIntegerForKey("key"));
    this->modifiers = dict->getIntegerForKey("modifiers");
    if (version > 1)
        this->mouse = static_cast<decltype(this->mouse)>(
            dict->getIntegerForKey("click")
        );
}

std::size_t std::hash<Keybind>::operator()(Keybind const& key) const {
    return (key.key << 8) + (key.modifiers << 4) + (key.mouse);
}

std::size_t std::hash<keybind_id>::operator()(keybind_id const& key) const {
    return std::hash<std::string>()(key.value);
}

bool KeybindCallback::operator==(KeybindCallback const& other) const {
    return this->id == other.id;
}

void KeybindManager::encodeDataTo(DS_Dictionary* dict) {
    BetterEdit::log() << "Saving keybinds" << log_end();

    dict->setIntegerForKey("double-click-interval", this->m_nDoubleClickInterval);
    dict->setIntegerForKey("version", this->getVersion());
    
    STEP_SUBDICT(dict, "binds",
        for (auto const& [type, val] : m_mCallbacks) {
            STEP_SUBDICT(dict, CCString::createWithFormat("k%d", type)->getCString(),
                for (auto const& cb : val) {
                    STEP_SUBDICT(dict, cb->id.c_str(),
                        int ix = 0;
                        for (auto const& bind : getKeybindsForCallback(type, cb)) {
                            auto k = CCString::createWithFormat("k%d", ix++)->getCString();
                            STEP_SUBDICT(dict, k,
                                bind.save(dict);
                            );
                        }
                    );
                }
            );
        }
    );
}

void KeybindManager::dataLoaded(DS_Dictionary* dict) {
    BetterEdit::log() << "Loading keybinds" << log_end();
    
    if (DSdictHasKey(dict, "double-click-interval"))
        this->m_nDoubleClickInterval = dict->getIntegerForKey("double-click-interval");
    
    int ver = 1;
    if (DSdictHasKey(dict, "version"))
        ver = dict->getIntegerForKey("version");
    
    STEP_SUBDICT_NC(dict, "binds",
        for (auto const& typeKey : dict->getAllKeys()) {
            auto type = static_cast<KeybindType>(std::stoi(typeKey.substr(1)));

            m_mLoadedBinds[type] = std::unordered_map<keybind_id, KeybindList>();

            STEP_SUBDICT_NC(dict, typeKey.c_str(),
                for (auto const& nameKey : dict->getAllKeys()) {
                    m_mLoadedBinds[type][nameKey] = KeybindList();

                    STEP_SUBDICT_NC(dict, nameKey.c_str(),
                        for (auto const& key : dict->getAllKeys()) {
                            STEP_SUBDICT_NC(dict, key.c_str(),
                                m_mLoadedBinds[type][nameKey].insert(Keybind(dict, ver));
                            );
                        }
                    );
                }
            );
        }
    );
}

void KeybindManager::firstLoad() {
}

bool KeybindManager::init() {
    if (!CCNode::init())
        return false;
    
    this->m_sFileName = "BEKeybindManager.dat";

    this->setup();
    this->loadDefaultKeybinds();
    
    return true;
}

bool KeybindManager::initGlobal() {
    g_manager = new KeybindManager;

    if (g_manager && g_manager->init())
        return true;

    CC_SAFE_DELETE(g_manager);
    return false;
}

void KeybindManager::loadDefaultKeybinds() {
    this->addPlayKeybind({ "Pause", "gd.play.pause",
        [](PlayLayer* pl, EditorUI* ui, bool push) -> bool {
            if (!push) return false;
            if (ui) ui->onPause(nullptr);
            if (pl) pl->m_uiLayer->onPause(nullptr);
            return false;
        }
    }, {{ KEY_Escape, 0 }});
    
    this->addPlayKeybind({ "Jump P1", "gd.play.jump_p1",
        [](PlayLayer* pl, EditorUI* ui, bool push) -> bool {
            if (push) {
                if (ui) ui->m_pEditorLayer->pushButton(0, true);
                else pl->pushButton(0, true);
            } else {
                if (ui) ui->m_pEditorLayer->releaseButton(0, true);
                else pl->releaseButton(0, true);
            }
            return false;
        }
    }, {{ KEY_Space, 0 }});
    
    this->addPlayKeybind({ "Jump P2", "gd.play.jump_p2",
        [](PlayLayer* pl, EditorUI* ui, bool push) -> bool {
            if (push) {
                if (ui) ui->m_pEditorLayer->pushButton(0, false);
                else pl->pushButton(0, false);
            } else {
                if (ui) ui->m_pEditorLayer->releaseButton(0, false);
                else pl->releaseButton(0, false);
            }
            return false;
        }
    }, {{ KEY_Up, 0 }});

    this->addPlayKeybind({ "Place Checkpoint", "gd.play.place_checkpoint",
        [](PlayLayer* pl, bool push) -> bool {
            if (push && pl->m_isPracticeMode) {
                pl->m_uiLayer->onCheck(nullptr);
            }
            return false;
        }
    }, {{ KEY_Z, 0 }});

    this->addPlayKeybind({ "Delete Checkpoint", "gd.play.delete_checkpoint",
        [](PlayLayer* pl, bool push) -> bool {
            if (push && pl->m_isPracticeMode) {
                pl->m_uiLayer->onDeleteCheck(nullptr);
            }
            return false;
        }
    }, {{ KEY_X, 0 }});

    this->addPlayKeybind({ "Practice Mode", "gd.play.practice_mode",
        [](PlayLayer* pl, bool push) -> bool {
            if (push)
                pl->togglePracticeMode(!pl->m_isPracticeMode);
            return false;
        },
        false
    }, {});

    this->addEditorKeybind({ "Build Mode", "gd.edit.build_mode",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->toggleMode(ui->m_pBuildModeBtn);
            return false;
        }, "editor.ui", false
    }, {{ KEY_One, 0 }});

    this->addEditorKeybind({ "Edit Mode", "gd.edit.edit_mode",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->toggleMode(ui->m_pEditModeBtn);
            return false;
        }, "editor.ui", false
    }, {{ KEY_Two, 0 }});

    this->addEditorKeybind({ "Delete Mode", "gd.edit.delete_mode",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->toggleMode(ui->m_pDeleteModeBtn);
            return false;
        }, "editor.ui", false
    }, {{ KEY_Three, 0 }});

    this->addEditorKeybind({ "View Mode", "betteredit.view_mode",
        [](EditorUI* ui) -> bool {
            if (
                ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying &&
                !BetterEdit::getDisableVisibilityTab()
            )
                ui->toggleMode(ui->m_pBuildModeBtn->getParent()->getChildByTag(4));
        
            return false;
        }, "editor.ui", false
    }, {{ KEY_Four, 0 }});

    this->addEditorKeybind({ "Swipe modifier", "gd.edit.swipe_modifier",
        true, "editor.global"
    }, {{ KEY_None, Keybind::kmShift }});

    this->addEditorKeybind({ "Move modifier", "gd.edit.move_modifier",
        true, "editor.global"
    }, {{ KEY_Space, 0 }});

    this->addEditorKeybind({ "Rotate CCW", "gd.edit.rotate_ccw",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->transformObjectCall(kEditCommandRotateCCW);
            return false;
        }, "editor.modify"
    }, {{ KEY_Q, 0 }});

    this->addEditorKeybind({ "Rotate CW", "gd.edit.rotate_cw",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->transformObjectCall(kEditCommandRotateCW);
            return false;
        }, "editor.modify"
    }, {{ KEY_E, 0 }});

    this->addEditorKeybind({ "Flip X", "gd.edit.flip_x",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->transformObjectCall(kEditCommandFlipX);
            return false;
        }, "editor.modify"
    }, {{ KEY_Q, Keybind::kmAlt }});

    this->addEditorKeybind({ "Flip Y", "gd.edit.flip_y",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->transformObjectCall(kEditCommandFlipY);
            return false;
        }, "editor.modify"
    }, {{ KEY_E, Keybind::kmAlt }});

    this->addEditorKeybind({ "Delete Selected", "gd.edit.delete_selected",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->onDeleteSelected(nullptr);
            return false;
        }, "editor.modify", false
    }, {{ KEY_Delete, 0 }});

    this->addEditorKeybind({ "Undo", "gd.edit.undo",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->undoLastAction(nullptr);
            return false;
        }, "editor.global"
    }, {{ KEY_Z, Keybind::kmControl }});

    this->addEditorKeybind({ "Redo", "gd.edit.redo",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->redoLastAction(nullptr);
            return false;
        }, "editor.global"
    }, {{ KEY_Z, Keybind::kmControl | Keybind::kmShift }});

    this->addEditorKeybind({ "Deselect", "gd.edit.deselect",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->deselectAll();
            return false;
        }, "editor.select", false
    }, {{ KEY_D, Keybind::kmAlt }});

    this->addEditorKeybind({ "Copy", "gd.edit.copy",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->onCopy(nullptr);
            return false;
        }, "editor.modify", false
    }, {{ KEY_C, Keybind::kmControl }});

    this->addEditorKeybind({ "Cut", "gd.edit.cut",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying) {
                ui->onCopy(nullptr);
                ui->onDeleteSelected(nullptr);
            }
            return false;
        }, "editor.modify", false
    }, {});

    this->addEditorKeybind({ "Paste", "gd.edit.paste",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->onPaste(nullptr);
            return false;
        }, "editor.modify"
    }, {{ KEY_V, Keybind::kmControl }});

    this->addEditorKeybind({ "Duplicate", "gd.edit.copy_and_paste",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->onDuplicate(nullptr);
            return false;
        }, "editor.modify"
    }, {{ KEY_D, Keybind::kmControl }});

    this->addEditorKeybind({ "Rotate", "gd.edit.toggle_rotate",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->toggleEnableRotate(nullptr);
            return false;
        }, "editor.modify"
    }, {{ KEY_R, 0 }});

    this->addEditorKeybind({ "Free Move", "gd.edit.toggle_free_move",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->toggleFreeMove(nullptr);
            return false;
        }, "editor.modify"
    }, {{ KEY_F, 0 }});

    this->addEditorKeybind({ "Swipe", "gd.edit.toggle_swipe",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->toggleSwipe(nullptr);
            return false;
        }, "editor.modify"
    }, {{ KEY_T, 0 }});

    this->addEditorKeybind({ "Snap", "gd.edit.toggle_snap",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->toggleSnap(nullptr);
            return false;
        }, "editor.modify"
    }, {{ KEY_G, 0 }});

    this->addEditorKeybind({ "Playtest", "gd.edit.playtest",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode == kPlaybackModePlaying)
                ui->onStopPlaytest(nullptr);
            else
                ui->onPlaytest(nullptr);

            return false;
        }, "editor.global", false
    }, {{ KEY_Enter, 0 }});

    this->addEditorKeybind({ "Pause Playtest", "gd.edit.pause_playtest",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode == kPlaybackModePlaying)
                ui->onPlaytest(nullptr);
            return false;
        }, "editor.global", false
    }, {});

    this->addEditorKeybind({ "Playback Music", "gd.edit.playback_music",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->onPlayback(nullptr);
            return false;
        }, "editor.global", false
    }, {{ KEY_Enter, Keybind::kmControl }});

    this->addEditorKeybind({ "Previous Build Tab", "gd.edit.prev_build_tab",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode == kPlaybackModePlaying)
                return false;
            
            auto t = ui->m_nSelectedTab - 1;
            if (t < 0)
                t = ui->m_pTabsArray->count() - 1;
            ui->selectBuildTab(t);
            return false;
        }, "editor.ui"
    }, {{ KEY_F1, 0 }});

    this->addEditorKeybind({ "Next Build Tab",  "gd.edit.next_build_tab",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode == kPlaybackModePlaying)
                return false;

            auto t = ui->m_nSelectedTab + 1;
            if (t > static_cast<int>(ui->m_pTabsArray->count() - 1))
                t = 0;
            ui->selectBuildTab(t);
            return false;
        }, "editor.ui"
    }, {{ KEY_F2, 0 }});

    this->addEditorKeybind({ "Next Group", "gd.edit.next_layer",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->onGroupUp(nullptr);
            return false;
        }, "editor.ui"
    }, {{ KEY_Right, 0 }});

    this->addEditorKeybind({ "Previous Group", "gd.edit.prev_layer",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->onGroupDown(nullptr);
            return false;
        }, "editor.ui"
    }, {{ KEY_Left, 0 }});

    this->addEditorKeybind({ "Scroll Up",  "gd.edit.scroll_up",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->moveGameLayer({ 0.0f, 10.0f });
            return false;
        }, "editor.ui"
    }, {{ KEY_OEMPlus, 0 }});

    this->addEditorKeybind({ "Scroll Down", "gd.edit.scroll_down",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->moveGameLayer({ 0.0f, -10.0f });
            return false;
        }, "editor.ui"
    }, {{ KEY_OEMMinus, 0 }});

    this->addEditorKeybind({ "Zoom In", "gd.edit.zoom_in",
        [](EditorUI* ui) -> bool {
            ui->zoomIn(nullptr);
            return false;
        }, "editor.ui"
    }, {{ KEY_OEMPlus, Keybind::kmShift }});

    this->addEditorKeybind({ "Zoom Out", "gd.edit.zoom_out",
        [](EditorUI* ui) -> bool {
            ui->zoomOut(nullptr);
            return false;
        }, "editor.ui"
    }, {{ KEY_OEMMinus, Keybind::kmShift }});

    this->addEditorKeybind({ "Object Left", "gd.edit.move_obj_left",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->moveObjectCall(kEditCommandLeft);
            return false;
        }, "editor.move"
    }, {{ KEY_A, 0 }});

    this->addEditorKeybind({ "Object Right", "gd.edit.move_obj_right",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->moveObjectCall(kEditCommandRight);
            return false;
        }, "editor.move"
    }, {{ KEY_D, 0 }});

    this->addEditorKeybind({ "Object Up", "gd.edit.move_obj_up",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->moveObjectCall(kEditCommandUp);
            return false;
        }, "editor.move"
    }, {{ KEY_W, 0 }});

    this->addEditorKeybind({ "Object Down", "gd.edit.move_obj_down",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->moveObjectCall(kEditCommandDown);
            return false;
        }, "editor.move"
    }, {{ KEY_S, 0 }});

    this->addEditorKeybind({ "Object Left Small", "gd.edit.move_obj_small_left",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->moveObjectCall(kEditCommandSmallLeft);
            return false;
        }, "editor.move"
    }, {{ KEY_A, Keybind::kmShift }});

    this->addEditorKeybind({ "Object Right Small", "gd.edit.move_obj_small_right",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->moveObjectCall(kEditCommandSmallRight);
            return false;
        }, "editor.move"
    }, {{ KEY_D, Keybind::kmShift }});

    this->addEditorKeybind({ "Object Up Small", "gd.edit.move_obj_small_up",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->moveObjectCall(kEditCommandSmallUp);
            return false;
        }, "editor.move"
    }, {{ KEY_W, Keybind::kmShift }});

    this->addEditorKeybind({ "Object Down Small", "gd.edit.move_obj_small_down",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->moveObjectCall(kEditCommandSmallDown);
            return false;
        }, "editor.move"
    }, {{ KEY_S, Keybind::kmShift }});
    
    this->addEditorKeybind({ "Object Left Tiny", "gd.edit.move_obj_tiny_left",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->moveObjectCall(kEditCommandTinyLeft);
            return false;
        }, "editor.move"
    }, {});

    this->addEditorKeybind({ "Object Right Tiny", "gd.edit.move_obj_tiny_right",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->moveObjectCall(kEditCommandTinyRight);
            return false;
        }, "editor.move"
    }, {});

    this->addEditorKeybind({ "Object Up Tiny", "gd.edit.move_obj_tiny_up",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->moveObjectCall(kEditCommandTinyUp);
            return false;
        }, "editor.move"
    }, {});

    this->addEditorKeybind({ "Object Down Tiny", "gd.edit.move_obj_tiny_down",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->moveObjectCall(kEditCommandTinyDown);
            return false;
        }, "editor.move"
    }, {});

    this->addEditorKeybind({ "Object Left Big", "gd.edit.move_obj_big_left",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->moveObjectCall(kEditCommandBigLeft);
            return false;
        }, "editor.move"
    }, {});

    this->addEditorKeybind({ "Object Right Big", "gd.edit.move_obj_big_right",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->moveObjectCall(kEditCommandBigRight);
            return false;
        }, "editor.move"
    }, {});

    this->addEditorKeybind({ "Object Up Big", "gd.edit.move_obj_big_up",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->moveObjectCall(kEditCommandBigUp);
            return false;
        }, "editor.move"
    }, {});

    this->addEditorKeybind({ "Object Down Big", "gd.edit.move_obj_big_down",
        [](EditorUI* ui) -> bool {
            if (ui->m_pEditorLayer->m_ePlaybackMode != kPlaybackModePlaying)
                ui->moveObjectCall(kEditCommandBigDown);
            return false;
        }, "editor.move"
    }, {});
}

std::vector<KeybindCallback*> const& KeybindManager::getCallbacks(KeybindType type) {
    return m_mCallbacks[type];
}

std::map<std::string, KeybindManager::CallbackList> KeybindManager::getCallbacksSorted(KeybindType type) {
    std::map<std::string, CallbackList> m_mSort;

    for (auto const& b : getCallbacks(type))
        m_mSort[b->subcategory].push_back(b);
    
    return m_mSort;
}

std::string KeybindManager::getCategoryName(std::string const& category) {
    // switch (hash(category.c_str())) {
    //     case hash("editor.modify"): return "Modify";
    //     case hash("editor.ui"): return "UI";
    //     case hash("editor.global"): return "Global";
    // }

    return category;
}

std::vector<KeybindCallback*> KeybindManager::getAllCallbacks() {
    CallbackList res;

    for (auto const& [_, c] : m_mCallbacks)
        res.insert(res.end(), c.begin(), c.end());

    return res;
}

KeybindList KeybindManager::getKeybindsForCallback(
    KeybindType type, KeybindCallback* cb
) {
    std::set<Keybind> res;

    for (auto & [key, vals] : m_mKeybinds) {
        for (auto & val : vals)
            if (val.type == type && val.bind == cb)
                res.insert(key);
    }

    return res;
}

size_t KeybindManager::getIndexOfCallback(KeybindType type, KeybindCallback* cb) {
    size_t ix = 0;
    for (auto const& cb_ : this->m_mCallbacks[type]) {
        if (cb_ == cb)
            return ix;
        
        ix++;
    }
    return 0;
}

KeybindList KeybindManager::getLoadedBinds(KeybindType type, KeybindCallback* cb, bool* exists) {
    KeybindList res;
    *exists = false;

    if (m_mLoadedBinds[type].count(cb->id)) {
        *exists = true;
        return m_mLoadedBinds[type][cb->id];
    }

    return res;
}

void KeybindManager::addCallback(
    KeybindCallback* cb,
    KeybindType type,
    KeybindList const& binds
) {
    auto index = m_mCallbacks[type].size();

    for (auto const& [_, calls] : m_mCallbacks)
        for (auto const& call : calls)
            if (call->id == cb->id) {
                // le memory leak
                delete cb;
                return;
            }

    // cb->id = m_mCallbacks[type].size();
    cb->defaults = binds;

    m_mCallbacks[type].push_back(cb);

    bool custom;
    auto cbinds = getLoadedBinds(type, cb, &custom);

    if (custom) {
        for (auto bind : cbinds) {
            this->addKeybind(type, cb, bind);
        }
    } else
        for (auto bind : binds) {
            this->addKeybind(type, cb, bind);
        }
}

void KeybindManager::addEditorKeybind(
    KeybindEditor cb,
    KeybindList const& binds
) {
    this->addCallback(new KeybindEditor(cb), kKBEditor, binds);
}

void KeybindManager::addPlayKeybind(
    KeybindPlayLayer cb,
    KeybindList const& binds
) {
    this->addCallback(new KeybindPlayLayer(cb), kKBPlayLayer, binds);
}

void KeybindManager::addGlobalKeybind(
    KeybindGlobal cb,
    KeybindList const& binds
) {
    this->addCallback(new KeybindGlobal(cb), kKBGlobal, binds);
}

void KeybindManager::addKeybind(KeybindType type, KeybindCallback* cb, Keybind const& bind) {
    if (m_mKeybinds.count(bind) && m_mKeybinds[bind].size())
        m_mKeybinds[bind].push_back({ type, cb });
    else
        m_mKeybinds[bind] = {{ type, cb }};
}

void KeybindManager::removeKeybind(KeybindType type, KeybindCallback* cb, Keybind const& bind) {
    if (m_mKeybinds.count(bind) && m_mKeybinds[bind].size()) {
        std::vector<KeybindManager::Target>::iterator iter;
        for (iter = m_mKeybinds[bind].begin(); iter != m_mKeybinds[bind].end(); ) {
            if (iter->bind == cb)
                iter = m_mKeybinds[bind].erase(iter);
            else
                iter++;
        }
    }
}

void KeybindManager::editKeybind(KeybindType type, KeybindCallback* cb, Keybind const& old, Keybind const& bind) {
    this->removeKeybind(type, cb, old);
    this->addKeybind(type, cb, bind);
}

void KeybindManager::clearKeybinds(KeybindType type, KeybindCallback* cb) {
    for (auto & [key, vals] : m_mKeybinds) {
        auto ix = 0;
        for (auto & val : vals) {
            if (val.type == type && val.bind == cb) {
                vals.erase(vals.begin() + ix);
                ix--;
                if (!vals.size())
                    m_mKeybinds.erase(key);
            }
            ix++;
        }
    }
}

void KeybindManager::clearCallbacks(Keybind const& bind) {
    if (bind.key != KEY_None && m_mKeybinds.count(bind))
        m_mKeybinds.erase(bind);
}

std::vector<KeybindManager::Target> KeybindManager::getAllCallbacksForKeybind(Keybind const& bind) {
    if (!m_mKeybinds.count(bind))
        return {};
    
    std::vector<KeybindManager::Target> res;

    for (auto & target : m_mKeybinds[bind])
        res.push_back(target);
    
    return res;
}

KeybindManager::CallbackList KeybindManager::getCallbacksForKeybind(KeybindType type, Keybind const& bind) {
    if (!m_mKeybinds.count(bind))
        return {};
    
    CallbackList res;

    for (auto & target : m_mKeybinds[bind])
        if (target.type == type)
            res.push_back(target.bind);
    
    return res;
}

void KeybindManager::invokeEditor(Target const& target, EditorUI* ui, bool keydown, bool onlyPlay) {
    if (target.bind->modifier)
        return;

    switch (target.type) {
        case kKBEditor: {
            if (onlyPlay) break;
            auto c = as<KeybindEditor*>(target.bind);
            if (c->call_b)
                c->call_b(ui, keydown);
            else if (keydown)
                c->call(ui);
        } break;
        
        case kKBPlayLayer: {
            if (as<KeybindPlayLayer*>(target.bind)->editor)
                as<KeybindPlayLayer*>(target.bind)->call_e(nullptr, ui, keydown);
        } break;
    }
}

void KeybindManager::invokePlay(Target const& target, PlayLayer* pl, bool keydown) {
    if (target.bind->modifier)
        return;

    switch (target.type) {
        case kKBPlayLayer: {
            auto c = as<KeybindPlayLayer*>(target.bind);
            if (c->call_e)
                c->call_e(pl, nullptr, keydown);
            else
                c->call(pl, keydown);
        } break;
    }
}

void KeybindManager::executeEditorCallbacks(Keybind const& bind, EditorUI* ui, bool keydown, bool onlyPlay) {
    if (!m_mKeybinds.count(bind))
        return;

    auto targets = m_mKeybinds[bind];
    if (!targets.size()) {
        if (bind.modifiers) {
            if (bind.key != KEY_None)
                targets = m_mKeybinds[Keybind(bind.key, 0)];
            else if (bind.mouse != kMouseButtonNone)
                targets = m_mKeybinds[Keybind(bind.mouse, 0)];
        }
    }
    for (auto & target : targets) {
        this->invokeEditor(target, ui, keydown, onlyPlay);
    }
}

void KeybindManager::executePlayCallbacks(Keybind const& bind, PlayLayer* pl, bool keydown) {
    if (!m_mKeybinds.count(bind))
        return;

    auto targets = m_mKeybinds[bind];
    if (!targets.size()) {
        if (bind.modifiers) {
            if (bind.key != KEY_None)
                targets = m_mKeybinds[Keybind(bind.key, 0)];
            else if (bind.mouse != kMouseButtonNone)
                targets = m_mKeybinds[Keybind(bind.mouse, 0)];
        }
    }
    for (auto & target : targets) {
        this->invokePlay(target, pl, keydown);
    }
}

void KeybindManager::resetToDefault(KeybindType type, KeybindCallback* cb, bool resetBinds) {
    if (resetBinds && cb->defaults.size()) {
        this->clearKeybinds(type, cb);
        for (auto const& bind : cb->defaults)
            this->addKeybind(type, cb, bind);
    }

    cb->repeatStart = BetterEdit::getKeybindRepeatStart();
    cb->repeatInterval = BetterEdit::getKeybindRepeatInterval();
    cb->repeat = BetterEdit::getKeybindRepeatEnabled();
    cb->repeatChanged = false;
}

void KeybindManager::resetAllToDefaults() {
    for (auto const& [type, cbs] : m_mCallbacks)
        for (auto const& cb : cbs)
            this->resetToDefault(type, cb);
}

void KeybindManager::resetUnmodifiedRepeatTimes() {
    for (auto const& [type, cbs] : m_mCallbacks)
        for (auto const& cb : cbs)
            if (!cb->repeatChanged)
                this->resetToDefault(type, cb, false);
}

void KeybindManager::handleRepeats(float dt) {
    if (
        !BetterEdit::isEditorInitialized() &&
        !GameManager::sharedState()->getPlayLayer()
    ) {
        m_mHeldKeys.clear();
        m_mHeldMouse.clear();
        return;
    }

    std::unordered_map<Keybind, float> held;
    for (auto & [key, time] : m_mHeldKeys) {
        held.insert({ Keybind(key), time });
    }
    for (auto & [mouse, time] : m_mHeldMouse) {
        held.insert({ Keybind(mouse), time });
    }

    for (auto & [k, time] : held) {
        time += dt * 1000.0f;
        for (auto const& [type, bind] : m_mKeybinds[k]) {
            if (bind->repeatable && bind->repeat) {
                if (
                    (time - bind->repeatStart) >=
                    0
                ) {
                    time -= bind->repeatInterval;
                    switch (type) {
                        case kKBEditor: {
                            auto ui = LevelEditorLayer::get()->getEditorUI();
                            if (ui)
                                this->executeEditorCallbacks(k, ui, true);
                        } break;
                        
                        case kKBPlayLayer:
                            auto pl = PlayLayer::get();

                            if (pl)
                                this->executePlayCallbacks(k, pl, true);
                            break;
                    }
                }
            }
        }
    }
}

void KeybindManager::registerKeyPress(enumKeyCodes key, bool down) {
    if (down) {
        if (!m_mHeldKeys.count(key))
            m_mHeldKeys[key] = 0;
    } else {
        m_mHeldKeys.erase(key);
    }
}

void KeybindManager::registerMousePress(MouseButton btn, bool down) {
    if (down) {
        if (!m_mHeldMouse.count(btn))
            m_mHeldMouse[btn] = 0;
    } else {
        m_mHeldMouse.erase(btn);
    }
}

decltype(KeybindManager::m_mKeybinds) const& KeybindManager::getAllKeybinds() {
    return m_mKeybinds;
}

KeybindManager::Target KeybindManager::getCallbackByName(std::string const& cbName) {
    for (auto const& [type, binds] : this->m_mCallbacks) {
        for (auto const& bind : binds) {
            if (bind->name == cbName)
                return {
                    type,
                    bind
                };
        }
    }

    return KeybindManager::Target();
}

KeybindManager::Target KeybindManager::getTargetByID(keybind_id const& cbID) {
    for (auto const& [type, binds] : this->m_mCallbacks) {
        for (auto const& bind : binds) {
            if (bind->id == cbID)
                return {
                    type,
                    bind
                };
        }
    }
    return KeybindManager::Target();
}

bool KeybindManager::isModifierPressed(keybind_id const& id) {
    auto cb = this->getTargetByID(id);
    if (!cb.bind) return false;

    auto kbs = this->getKeybindsForCallback(cb.type, cb.bind);

    for (auto const& kb : kbs) {
        bool res = true;

        if ((
            kb.key != KEY_None &&
            !this->m_mHeldKeys.count(kb.key)
        ) || (
            kb.mouse != kMouseButtonNone &&
            !this->m_mHeldMouse.count(kb.mouse)
        ))
            res = false;
        
        if (
            static_cast<bool>(kb.modifiers & kb.kmControl) !=
            static_cast<bool>(this->m_mHeldKeys.count(KEY_Control))
        )
            res = false;
        
        if (
            static_cast<bool>(kb.modifiers & kb.kmShift) !=
            static_cast<bool>(this->m_mHeldKeys.count(KEY_Shift))
        )
            res = false;
        
        if (
            static_cast<bool>(kb.modifiers & kb.kmAlt) !=
            static_cast<bool>(this->m_mHeldKeys.count(KEY_Alt))
        )
            res = false;
        
        if (res) return true;
    }
    
    return false;
}

void KeybindManager::setDoubleClickInterval(int i) {
    this->m_nDoubleClickInterval = i;
}

int KeybindManager::getDoubleClickInterval() const {
    return this->m_nDoubleClickInterval;
}

constexpr int KeybindManager::getVersion() const {
    return s_nVersion;
}

bool KeybindManager::isAllowedMouseButton(MouseButton btn) const {
    switch (btn) {
        case kMouseButtonPrev: case kMouseButtonNext:
        case kMouseButtonRight: case kMouseButtonMiddle:
            return true;
    }
    return false;
}

void KeybindManager::invokeCallback(
    keybind_id const& cbID,
    EditorUI* ui,
    PlayLayer* pl
) {
    auto target = this->getTargetByID(cbID);

    switch (target.type) {
        case kKBEditor:
            this->invokeEditor(target, ui, true, false);
            break;

        case kKBPlayLayer:
            this->invokePlay(target, pl, true);
            break;
    }
}

KeybindManager* KeybindManager::get() {
    return g_manager;
}

