#include "KeybindManager.hpp"

KeybindManager* g_manager;

bool Keybind::operator==(Keybind const& other) const {
    return
        other.key == this->key &&
        other.modifiers == this->modifiers;
}

bool Keybind::operator<(Keybind const& other) const {
    return this->key < other.key;
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

std::size_t std::hash<Keybind>::operator()(Keybind const& key) const {
    return (key.key << 8) + (key.modifiers << 4) + (key.click);
}

bool KeybindCallback::operator==(KeybindCallback const& other) const {
    return this->id == other.id;
}

void KeybindManager::encodeDataTo(DS_Dictionary*) {
}

void KeybindManager::dataLoaded(DS_Dictionary*) {
}

void KeybindManager::firstLoad() {
}

bool KeybindManager::init() {
    if (!CCNode::init())
        return false;
    
    this->m_sFileName = "BEKeybindManager.dat";
    this->loadDefaultKeybinds();

    this->setup();
    
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
    this->addEditorKeybind({ "Build Mode", [](EditorUI* ui) -> bool {
        ui->toggleMode(ui->m_pBuildModeBtn);
        return false;
    }}, {{ KEY_One, 0 }});

    this->addEditorKeybind({ "Edit Mode", [](EditorUI* ui) -> bool {
        ui->toggleMode(ui->m_pEditModeBtn);
        return false;
    }}, {{ KEY_Two, 0 }});

    this->addEditorKeybind({ "Delete Mode", [](EditorUI* ui) -> bool {
        ui->toggleMode(ui->m_pDeleteModeBtn);
        return false;
    }}, {{ KEY_Three, 0 }});

    this->addEditorKeybind({ "Swipe modifier", [](EditorUI* ui) -> bool {
        return false;
    }}, {{ KEY_None, Keybind::kmShift }});

    this->addEditorKeybind({ "Move modifier", [](EditorUI* ui) -> bool {
        return false;
    }}, {{ KEY_Space, 0 }});

    this->addEditorKeybind({ "Rotate CCW", [](EditorUI* ui) -> bool {
        ui->transformObjectCall(kEditCommandRotateCCW);
        return false;
    }}, {{ KEY_Q, 0 }});

    this->addEditorKeybind({ "Rotate CW", [](EditorUI* ui) -> bool {
        ui->transformObjectCall(kEditCommandRotateCW);
        return false;
    }}, {{ KEY_E, 0 }});

    this->addEditorKeybind({ "Flip X", [](EditorUI* ui) -> bool {
        ui->transformObjectCall(kEditCommandFlipX);
        return false;
    }}, {{ KEY_Q, Keybind::kmAlt }});

    this->addEditorKeybind({ "Flip Y", [](EditorUI* ui) -> bool {
        ui->transformObjectCall(kEditCommandFlipY);
        return false;
    }}, {{ KEY_E, Keybind::kmAlt }});

    this->addEditorKeybind({ "Delete Selected", [](EditorUI* ui) -> bool {
        ui->onDeleteSelected(nullptr);
        return false;
    }}, {{ KEY_E, Keybind::kmAlt }});

    this->addEditorKeybind({ "Undo", [](EditorUI* ui) -> bool {
        ui->undoLastAction(nullptr);
        return false;
    }}, {{ KEY_Z, Keybind::kmControl }});

    this->addEditorKeybind({ "Redo", [](EditorUI* ui) -> bool {
        ui->redoLastAction(nullptr);
        return false;
    }}, {{ KEY_Z, Keybind::kmControl | Keybind::kmShift }});

    this->addEditorKeybind({ "Deselect", [](EditorUI* ui) -> bool {
        ui->deselectAll();
        return false;
    }}, {{ KEY_D, Keybind::kmAlt }});

    this->addEditorKeybind({ "Copy", [](EditorUI* ui) -> bool {
        return false;
    }}, {{ KEY_C, Keybind::kmControl }});

    // this->addEditorKeybind({ "Cut", [](EditorUI* ui) -> bool {
    //     return false;
    // }}, {{ KEY_X, Keybind::kmControl }});

    this->addEditorKeybind({ "Paste", [](EditorUI* ui) -> bool {
        return false;
    }}, {{ KEY_V, Keybind::kmControl }});

    this->addEditorKeybind({ "Duplicate", [](EditorUI* ui) -> bool {
        return false;
    }}, {{ KEY_D, Keybind::kmControl }});

    // todo

    this->addEditorKeybind({ "Move Left", [](EditorUI* ui) -> bool {
        ui->moveObjectCall(kEditCommandLeft);
        return false;
    }}, {{ KEY_A, 0 }});

    this->addEditorKeybind({ "Move Right", [](EditorUI* ui) -> bool {
        ui->moveObjectCall(kEditCommandRight);
        return false;
    }}, {{ KEY_D, 0 }});

    this->addEditorKeybind({ "Move Up", [](EditorUI* ui) -> bool {
        ui->moveObjectCall(kEditCommandUp);
        return false;
    }}, {{ KEY_W, 0 }});

    this->addEditorKeybind({ "Move Down", [](EditorUI* ui) -> bool {
        ui->moveObjectCall(kEditCommandDown);
        return false;
    }}, {{ KEY_S, 0 }});

    this->addEditorKeybind({ "Move Small Left", [](EditorUI* ui) -> bool {
        ui->moveObjectCall(kEditCommandSmallLeft);
        return false;
    }}, {{ KEY_A, Keybind::kmShift }});

    this->addEditorKeybind({ "Move Small Right", [](EditorUI* ui) -> bool {
        ui->moveObjectCall(kEditCommandSmallRight);
        return false;
    }}, {{ KEY_D, Keybind::kmShift }});

    this->addEditorKeybind({ "Move Small Up", [](EditorUI* ui) -> bool {
        ui->moveObjectCall(kEditCommandSmallUp);
        return false;
    }}, {{ KEY_W, Keybind::kmShift }});

    this->addEditorKeybind({ "Move Small Down", [](EditorUI* ui) -> bool {
        ui->moveObjectCall(kEditCommandSmallDown);
        return false;
    }}, {{ KEY_S, Keybind::kmShift }});
}

std::vector<KeybindCallback*> const& KeybindManager::getCallbacks(KeybindType type) {
    return m_mCallbacks[type];
}

KeybindManager::KeybindList KeybindManager::getKeybindsForCallback(
    KeybindType type, size_t ix
) {
    std::set<Keybind> res;

    for (auto & [key, vals] : m_mKeybinds) {
        for (auto & val : vals)
            if (val.type == type && val.index == ix)
                res.insert(key);
    }

    return res;
}

size_t KeybindManager::getIndexOfCallback(KeybindType type, KeybindCallback* cb) {
    size_t ix = 0;
    for (auto const& cb_ : this->m_mCallbacks[type]) {
        if (*cb_ == *cb)
            return ix;
        
        ix++;
    }
    return 0;
}

void KeybindManager::addCallback(
    KeybindCallback* cb,
    KeybindType type,
    KeybindManager::KeybindList const& binds
) {
    auto index = m_mCallbacks[type].size();

    cb->id = m_mCallbacks[type].size();

    m_mCallbacks[type].push_back(cb);

    for (auto bind : binds) {
        this->addKeybind(type, index, bind);
    }
}

void KeybindManager::addEditorKeybind(
    KeybindEditor cb,
    KeybindManager::KeybindList const& binds
) {
    this->addCallback(new KeybindEditor(cb), kKBEditor, binds);
}

void KeybindManager::addPlayKeybind(
    KeybindPlayLayer cb,
    KeybindManager::KeybindList const& binds
) {
    this->addCallback(new KeybindPlayLayer(cb), kKBPlayLayer, binds);
}

void KeybindManager::addGlobalKeybind(
    KeybindGlobal cb,
    KeybindManager::KeybindList const& binds
) {
    this->addCallback(new KeybindGlobal(cb), kKBGlobal, binds);
}

void KeybindManager::addKeybind(KeybindType type, size_t cbIx, Keybind const& bind) {
    if (m_mKeybinds.count(bind) && m_mKeybinds[bind].size())
        m_mKeybinds[bind].push_back({ type, cbIx });
    else
        m_mKeybinds[bind] = {{ type, cbIx }};
}

void KeybindManager::clearKeybinds(KeybindType type, size_t cbIx) {
    for (auto & [key, vals] : m_mKeybinds)
        for (auto & val : vals)
            if (val.type == type && val.index == cbIx)
                m_mKeybinds.erase(key);
}

void KeybindManager::executeEditorCallbacks(Keybind const& bind, EditorUI* ui) {
    if (!m_mKeybinds.count(bind))
        return;

    for (auto & target : m_mKeybinds[bind]) {
        as<KeybindEditor*>(m_mCallbacks[kKBEditor][target.index])->call(ui);
    }
}

KeybindManager* KeybindManager::get() {
    return g_manager;
}

