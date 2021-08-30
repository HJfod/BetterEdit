#include "KeybindManager.hpp"
#include <algorithm>
#include <functional>
#include "../VisibilityTab/loadVisibilityTab.hpp"

#define STEP_SUBDICT_NC(dict, key, ...)         \
    if (dict->stepIntoSubDictWithKey(key)) {    \
        __VA_ARGS__                             \
        dict->stepOutOfSubDict();               \
    }

#define STEP_SUBDICT(dict, key, ...)            \
    {                                           \
    if (!dict->stepIntoSubDictWithKey(key)) {   \
        dict->setSubDictForKey(key);            \
        if (!dict->stepIntoSubDictWithKey(key)) \
            return;                             \
    }                                           \
    __VA_ARGS__                                 \
    dict->stepOutOfSubDict();                   \
    }

KeybindManager* g_manager;

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
        other.click == this->click;
}

bool Keybind::operator<(Keybind const& other) const {
    return
        this->key < other.key ||
        this->modifiers < other.modifiers ||
        this->click << other.click;
}

std::string Keybind::toString() const {
    std::string res = "";

    if (this->modifiers & kmControl)  res += "Ctrl + ";
    if (this->modifiers & kmAlt)   res += "Alt + ";
    if (this->modifiers & kmShift) res += "Shift + ";

    switch (this->key) {
        case KEY_None:
            res = res.substr(0, res.size() - 3);
            break;
        
        case KEY_C:
            // because for some reason keyToString thinks C is a V
            res += "C";
            break;
        
        case static_cast<enumKeyCodes>(-1):
            res += "Unk";
            break;
        
        default:
            res += cocos2d::CCDirector::sharedDirector()
                ->getKeyboardDispatcher()
                ->keyToString(this->key);
    }

    return res;
}

void Keybind::save(DS_Dictionary* dict) const {
    dict->setIntegerForKey("key", this->key);
    dict->setIntegerForKey("modifiers", this->modifiers);
    dict->setIntegerForKey("click", this->click);
}

Keybind::Keybind() {
    this->key = KEY_None;
    this->modifiers = 0;
}

Keybind::Keybind(enumKeyCodes pressed) {
    this->key = pressed;

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

Keybind::Keybind(DS_Dictionary* dict) {
    this->key = static_cast<enumKeyCodes>(dict->getIntegerForKey("key"));
    this->modifiers = dict->getIntegerForKey("modifiers");
    this->click = static_cast<decltype(this->click)>(dict->getIntegerForKey("click"));
}

std::size_t std::hash<Keybind>::operator()(Keybind const& key) const {
    return (key.key << 8) + (key.modifiers << 4) + (key.click);
}

bool KeybindCallback::operator==(KeybindCallback const& other) const {
    return this->id == other.id;
}

void KeybindManager::encodeDataTo(DS_Dictionary* dict) {
    STEP_SUBDICT(dict, "binds",
        for (auto const& [type, val] : m_mCallbacks) {
            STEP_SUBDICT(dict, CCString::createWithFormat("k%d", type)->getCString(),
                for (auto const& cb : val) {
                    STEP_SUBDICT(dict, cb->name.c_str(),
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
    STEP_SUBDICT_NC(dict, "binds",
        for (auto const& typeKey : dict->getAllKeys()) {
            auto type = static_cast<KeybindType>(std::stoi(typeKey.substr(1)));

            m_mLoadedBinds[type] = std::unordered_map<std::string, KeybindList>();

            STEP_SUBDICT_NC(dict, typeKey.c_str(),
                for (auto const& nameKey : dict->getAllKeys()) {
                    m_mLoadedBinds[type][nameKey] = KeybindList();

                    STEP_SUBDICT_NC(dict, nameKey.c_str(),
                        for (auto const& key : dict->getAllKeys()) {
                            STEP_SUBDICT_NC(dict, key.c_str(),
                                m_mLoadedBinds[type][nameKey].insert(Keybind(dict));
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
    this->addPlayKeybind({ "Pause", [](PlayLayer* pl, EditorUI* ui, bool push) -> bool {
        if (!push) return false;
        if (ui) ui->onPause(nullptr);
        if (pl) pl->m_uiLayer->onPause(nullptr);
        return false;
    }}, {{ KEY_Escape, 0 }});
    
    this->addPlayKeybind({ "Jump P1", [](PlayLayer* pl, EditorUI* ui, bool push) -> bool {
        if (push) {
            if (ui) ui->m_pEditorLayer->pushButton(0, true);
            else pl->pushButton(0, true);
        } else {
            if (ui) ui->m_pEditorLayer->releaseButton(0, true);
            else pl->releaseButton(0, true);
        }
        return false;
    }}, {{ KEY_Space, 0 }});
    
    this->addPlayKeybind({ "Jump P2", [](PlayLayer* pl, EditorUI* ui, bool push) -> bool {
        if (push) {
            if (ui) ui->m_pEditorLayer->pushButton(0, false);
            else pl->pushButton(0, false);
        } else {
            if (ui) ui->m_pEditorLayer->releaseButton(0, false);
            else pl->releaseButton(0, false);
        }
        return false;
    }}, {{ KEY_Up, 0 }});

    this->addPlayKeybind({ "Place Checkpoint", [](PlayLayer* pl, bool push) -> bool {
        if (push) {
            pl->m_uiLayer->onCheck(nullptr);
        }
        return false;
    }}, {{ KEY_Z, 0 }});

    this->addPlayKeybind({ "Delete Checkpoint", [](PlayLayer* pl, bool push) -> bool {
        if (push) {
            pl->m_uiLayer->onDeleteCheck(nullptr);
        }
        return false;
    }}, {{ KEY_X, 0 }});

    this->addEditorKeybind({ "Build Mode", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->toggleMode(ui->m_pBuildModeBtn);
        return false;
    }, false}, {{ KEY_One, 0 }});

    this->addEditorKeybind({ "Edit Mode", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->toggleMode(ui->m_pEditModeBtn);
        return false;
    }, false}, {{ KEY_Two, 0 }});

    this->addEditorKeybind({ "Delete Mode", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->toggleMode(ui->m_pDeleteModeBtn);
        return false;
    }, false}, {{ KEY_Three, 0 }});

    this->addEditorKeybind({ "View Mode", [](EditorUI* ui) -> bool {
        if (
            !ui->m_pEditorLayer->m_bIsPlaybackMode &&
            !BetterEdit::getDisableVisibilityTab()
        )
            ui->toggleMode(ui->m_pBuildModeBtn->getParent()->getChildByTag(4));
    
        return false;
    }, false}, {{ KEY_Four, 0 }});

    this->addEditorKeybind({ "Swipe modifier", [](EditorUI* ui) -> bool {
        return false;
    }, false}, {{ KEY_None, Keybind::kmShift }});

    this->addEditorKeybind({ "Move modifier", [](EditorUI* ui, bool push) -> bool {
        ui->m_bSpaceKeyPressed = push;
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->m_bMoveModifier = push;
        return false;
    }}, {{ KEY_Space, 0 }});

    this->addEditorKeybind({ "Rotate CCW", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->transformObjectCall(kEditCommandRotateCCW);
        return false;
    }}, {{ KEY_Q, 0 }});

    this->addEditorKeybind({ "Rotate CW", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->transformObjectCall(kEditCommandRotateCW);
        return false;
    }}, {{ KEY_E, 0 }});

    this->addEditorKeybind({ "Flip X", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->transformObjectCall(kEditCommandFlipX);
        return false;
    }}, {{ KEY_Q, Keybind::kmAlt }});

    this->addEditorKeybind({ "Flip Y", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->transformObjectCall(kEditCommandFlipY);
        return false;
    }}, {{ KEY_E, Keybind::kmAlt }});

    this->addEditorKeybind({ "Delete Selected", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->onDeleteSelected(nullptr);
        return false;
    }, false}, {{ KEY_Delete, 0 }});

    this->addEditorKeybind({ "Undo", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->undoLastAction(nullptr);
        return false;
    }}, {{ KEY_Z, Keybind::kmControl }});

    this->addEditorKeybind({ "Redo", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->redoLastAction(nullptr);
        return false;
    }}, {{ KEY_Z, Keybind::kmControl | Keybind::kmShift }});

    this->addEditorKeybind({ "Deselect", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->deselectAll();
        return false;
    }, false}, {{ KEY_D, Keybind::kmAlt }});

    this->addEditorKeybind({ "Copy", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->onCopy(nullptr);
        return false;
    }, false}, {{ KEY_C, Keybind::kmControl }});

    this->addEditorKeybind({ "Cut", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode) {
            ui->onCopy(nullptr);
            ui->onDeleteSelected(nullptr);
        }
        return false;
    }, false}, {});

    this->addEditorKeybind({ "Paste", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->onPaste(nullptr);
        return false;
    }}, {{ KEY_V, Keybind::kmControl }});

    this->addEditorKeybind({ "Duplicate", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->onDuplicate(nullptr);
        return false;
    }}, {{ KEY_D, Keybind::kmControl }});

    this->addEditorKeybind({ "Rotate", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->toggleEnableRotate(nullptr);
        return false;
    }}, {{ KEY_R, 0 }});

    this->addEditorKeybind({ "Free Move", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->toggleFreeMove(nullptr);
        return false;
    }}, {{ KEY_F, 0 }});

    this->addEditorKeybind({ "Swipe", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->toggleSwipe(nullptr);
        return false;
    }}, {{ KEY_T, 0 }});

    this->addEditorKeybind({ "Snap", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->toggleSnap(nullptr);
        return false;
    }}, {{ KEY_G, 0 }});

    this->addEditorKeybind({ "Playtest", [](EditorUI* ui) -> bool {
        if (ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->onStopPlaytest(nullptr);
        else
            ui->onPlaytest(nullptr);

        return false;
    }}, {{ KEY_Enter, 0 }});

    this->addEditorKeybind({ "Playback Music", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->onPlayback(nullptr);
        return false;
    }}, {{ KEY_Enter, Keybind::kmControl }});

    this->addEditorKeybind({ "Previous Build Tab", [](EditorUI* ui) -> bool {
        if (ui->m_pEditorLayer->m_bIsPlaybackMode)
            return false;
        
        auto t = ui->m_nSelectedTab - 1;
        if (t < 0)
            t = ui->m_pTabsArray->count() - 1;
        ui->selectBuildTab(t);
        return false;
    }}, {{ KEY_F1, 0 }});

    this->addEditorKeybind({ "Next Build Tab", [](EditorUI* ui) -> bool {
        if (ui->m_pEditorLayer->m_bIsPlaybackMode)
            return false;

        auto t = ui->m_nSelectedTab + 1;
        if (t > static_cast<int>(ui->m_pTabsArray->count() - 1))
            t = 0;
        ui->selectBuildTab(t);
        return false;
    }}, {{ KEY_F2, 0 }});

    this->addEditorKeybind({ "Next Group", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->onGroupUp(nullptr);
        return false;
    }}, {{ KEY_Right, 0 }});

    this->addEditorKeybind({ "Previous Group", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->onGroupDown(nullptr);
        return false;
    }}, {{ KEY_Left, 0 }});

    this->addEditorKeybind({ "Scroll Up", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->moveGameLayer({ 0.0f, 10.0f });
        return false;
    }}, {{ KEY_OEMPlus, 0 }});

    this->addEditorKeybind({ "Scroll Down", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->moveGameLayer({ 0.0f, -10.0f });
        return false;
    }}, {{ KEY_OEMMinus, 0 }});

    this->addEditorKeybind({ "Zoom In", [](EditorUI* ui) -> bool {
        ui->zoomIn(nullptr);
        return false;
    }}, {{ KEY_OEMPlus, Keybind::kmShift }});

    this->addEditorKeybind({ "Zoom Out", [](EditorUI* ui) -> bool {
        ui->zoomOut(nullptr);
        return false;
    }}, {{ KEY_OEMMinus, Keybind::kmShift }});

    this->addEditorKeybind({ "Object Left", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->moveObjectCall(kEditCommandLeft);
        return false;
    }}, {{ KEY_A, 0 }});

    this->addEditorKeybind({ "Object Right", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->moveObjectCall(kEditCommandRight);
        return false;
    }}, {{ KEY_D, 0 }});

    this->addEditorKeybind({ "Object Up", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->moveObjectCall(kEditCommandUp);
        return false;
    }}, {{ KEY_W, 0 }});

    this->addEditorKeybind({ "Object Down", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->moveObjectCall(kEditCommandDown);
        return false;
    }}, {{ KEY_S, 0 }});

    this->addEditorKeybind({ "Object Left Small", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->moveObjectCall(kEditCommandSmallLeft);
        return false;
    }}, {{ KEY_A, Keybind::kmShift }});

    this->addEditorKeybind({ "Object Right Small", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->moveObjectCall(kEditCommandSmallRight);
        return false;
    }}, {{ KEY_D, Keybind::kmShift }});

    this->addEditorKeybind({ "Object Up Small", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->moveObjectCall(kEditCommandSmallUp);
        return false;
    }}, {{ KEY_W, Keybind::kmShift }});

    this->addEditorKeybind({ "Object Down Small", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->moveObjectCall(kEditCommandSmallDown);
        return false;
    }}, {{ KEY_S, Keybind::kmShift }});
    
    this->addEditorKeybind({ "Object Left Tiny", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->moveObjectCall(kEditCommandTinyLeft);
        return false;
    }}, {});

    this->addEditorKeybind({ "Object Right Tiny", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->moveObjectCall(kEditCommandTinyRight);
        return false;
    }}, {});

    this->addEditorKeybind({ "Object Up Tiny", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->moveObjectCall(kEditCommandTinyUp);
        return false;
    }}, {});

    this->addEditorKeybind({ "Object Down Tiny", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->moveObjectCall(kEditCommandTinyDown);
        return false;
    }}, {});

    this->addEditorKeybind({ "Object Left Big", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->moveObjectCall(kEditCommandBigLeft);
        return false;
    }}, {});

    this->addEditorKeybind({ "Object Right Big", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->moveObjectCall(kEditCommandBigRight);
        return false;
    }}, {});

    this->addEditorKeybind({ "Object Up Big", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->moveObjectCall(kEditCommandBigUp);
        return false;
    }}, {});

    this->addEditorKeybind({ "Object Down Big", [](EditorUI* ui) -> bool {
        if (!ui->m_pEditorLayer->m_bIsPlaybackMode)
            ui->moveObjectCall(kEditCommandBigDown);
        return false;
    }}, {});
}

std::vector<KeybindCallback*> const& KeybindManager::getCallbacks(KeybindType type) {
    return m_mCallbacks[type];
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

    if (m_mLoadedBinds[type].count(cb->name)) {
        *exists = true;
        return m_mLoadedBinds[type][cb->name];
    }

    return res;
}

void KeybindManager::addCallback(
    KeybindCallback* cb,
    KeybindType type,
    KeybindList const& binds
) {
    auto index = m_mCallbacks[type].size();

    cb->id = m_mCallbacks[type].size();
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

KeybindManager::CallbackList KeybindManager::getCallbacksForKeybind(KeybindType type, Keybind const& bind) {
    if (!m_mKeybinds.count(bind))
        return {};
    
    CallbackList res;

    for (auto & target : m_mKeybinds[bind])
        if (target.type == type)
            res.push_back(target.bind);
    
    return res;
}

void KeybindManager::executeEditorCallbacks(Keybind const& bind, EditorUI* ui, bool keydown, bool onlyPlay) {
    if (!m_mKeybinds.count(bind))
        return;

    for (auto & target : m_mKeybinds[bind]) {
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
}

void KeybindManager::executePlayCallbacks(Keybind const& bind, PlayLayer* pl, bool keydown) {
    if (!m_mKeybinds.count(bind))
        return;

    for (auto & target : m_mKeybinds[bind]) {
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
        return;
    }

    for (auto & [key, time] : m_mHeldKeys) {
        time += dt * 1000.0f;

        auto k = Keybind(key);

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

KeybindManager* KeybindManager::get() {
    return g_manager;
}

