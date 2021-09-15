#pragma once

#include "../../BetterEdit.hpp"

enum MouseButton {
    kMouseButtonNone        = -1,
    kMouseButtonLeft        = 0,
    kMouseButtonRight       = 1,
    kMouseButtonMiddle      = 2,
    kMouseButtonPrev        = 3,
    kMouseButtonNext        = 4,
    kMouseButtonDoubleClick = 19,
    kMouseButtonScrollUp    = 20,
    kMouseButtonScrollDown  = 21,
};

std::string keyToStringFixed(enumKeyCodes code);
std::string mouseToString(MouseButton btn);

struct Keybind {
    enumKeyCodes key = KEY_None;
    enum Modifiers : int {
        kmNone = 0, kmControl = 1, kmShift = 2, kmAlt = 4,
    };
    int modifiers;
    MouseButton mouse = kMouseButtonNone;

    bool operator==(Keybind const&) const;
    bool operator<(Keybind const&) const;

    std::string toString() const;
    void save(DS_Dictionary*) const;

    Keybind();
    Keybind(enumKeyCodes);
    Keybind(enumKeyCodes, Modifiers);
    Keybind(enumKeyCodes, int);
    Keybind(MouseButton);
    Keybind(MouseButton, int);
    Keybind(MouseButton, Modifiers);
    Keybind(DS_Dictionary*, int version);
};

struct keybind_id {
    std::string value;

    keybind_id();
    keybind_id(std::string const&);
    keybind_id(const char*);
    const char* c_str() const;
    bool operator==(keybind_id const&) const;
    keybind_id operator=(std::string const&);
};

namespace std {
    template<>
    struct hash<Keybind> {
        std::size_t operator()(Keybind const&) const;
    };
    template<>
    struct hash<keybind_id> {
        std::size_t operator()(keybind_id const&) const;
    };
}

using KeybindList = std::set<Keybind>;

struct KeybindCallback {
    keybind_id id = "";
    std::string name;
    std::string subcategory = "";
    KeybindList defaults;
    bool repeatable = true;
    bool repeatChanged = false;
    bool repeat = BetterEdit::getKeybindRepeatEnabled();
    int repeatInterval = BetterEdit::getKeybindRepeatInterval();
    int repeatStart = BetterEdit::getKeybindRepeatStart();
    bool modifier = false;

    bool operator==(KeybindCallback const&) const;

    virtual inline void z() {}
};

struct KeybindGlobal : public KeybindCallback {
    std::function<void()> call;

    inline KeybindGlobal(
        std::string const& keybind,
        std::function<void()> bind
    ) {
        this->name = keybind;
        this->call = bind;
    }
};

struct KeybindEditor : public KeybindCallback {
    std::function<bool(EditorUI*, bool)> call_b = nullptr;
    std::function<bool(EditorUI*)> call = nullptr;

    inline KeybindEditor(
        std::string const& keybind,
        std::string const& id,
        bool isModifier,
        std::string const& cat = ""
    ) {
        this->name = keybind;
        this->id = id;
        this->modifier = isModifier;
        this->repeatable = false;
        this->subcategory = cat;
    }
    
    inline KeybindEditor(
        std::string const& keybind,
        std::string const& id,
        decltype(call_b) bind,
        std::string const& cat = ""
    ) {
        this->name = keybind;
        this->id = id;
        this->call_b = bind;
        this->repeatable = false;
        this->subcategory = cat;
    }
    
    inline KeybindEditor(
        std::string const& keybind,
        std::string const& id,
        decltype(call) bind,
        std::string const& cat = "",
        bool isRepeatable = true
    ) {
        this->name = keybind;
        this->id = id;
        this->call = bind;
        this->repeatable = isRepeatable;
        this->subcategory = cat;
    }
};

struct KeybindPlayLayer : public KeybindCallback {
    std::function<bool(PlayLayer*, bool)> call;
    std::function<bool(PlayLayer*, EditorUI*, bool)> call_e;
    bool editor;

    inline KeybindPlayLayer(
        KeybindPlayLayer const& copy,
        bool alsoInEditor
    ) {
        this->name = copy.name;
        this->id = copy.id;
        this->call = copy.call;
        this->call_e = copy.call_e;
        this->id = copy.id;
        this->editor = alsoInEditor;
        this->repeatable = copy.repeatable;
    }

    inline KeybindPlayLayer(
        std::string const& keybind,
        std::string const& id,
        decltype(call) bind,
        bool repeat = true
    ) {
        this->name = keybind;
        this->id = id;
        this->call = bind;
        this->repeatable = repeat;
        this->editor = false;
    }

    inline KeybindPlayLayer(
        std::string const& keybind,
        std::string const& id,
        decltype(call_e) bind
    ) {
        this->name = keybind;
        this->id = id;
        this->call_e = bind;
        this->repeatable = false;
        this->editor = true;
    }
};

enum KeybindType {
    kKBEditor,
    kKBPlayLayer,
    kKBGlobal,
};

class KeybindManager : public GManager {
    public:
        using CallbackList = std::vector<KeybindCallback*>;
        struct Target {
            KeybindType type;
            KeybindCallback* bind = nullptr;
        };

    protected:
        std::unordered_map<KeybindType, CallbackList> m_mCallbacks;
        std::unordered_map<Keybind, std::vector<Target>> m_mKeybinds;
        // what the hell
        std::unordered_map<
            KeybindType,
            std::unordered_map<keybind_id, KeybindList>
        > m_mLoadedBinds;
        std::unordered_map<enumKeyCodes, float> m_mHeldKeys;
        std::unordered_map<MouseButton, float> m_mHeldMouse;
        int m_nDoubleClickInterval = 250;
        bool m_bPropagateKeyPresses = false;
        static constexpr int s_nVersion = 2;

        bool init();

        void addCallback(KeybindCallback*, KeybindType, KeybindList const&);
        KeybindList getLoadedBinds(KeybindType, KeybindCallback*, bool*);

        void invokeEditor(Target const&, EditorUI*, bool, bool);
        void invokePlay(Target const&, PlayLayer*, bool);

    public:
        void encodeDataTo(DS_Dictionary*) override;
        void dataLoaded(DS_Dictionary*) override;
        void firstLoad() override;

        void loadDefaultKeybinds();

        void addEditorKeybind(KeybindEditor, KeybindList const&);
        void addPlayKeybind(KeybindPlayLayer, KeybindList const&);
        void addGlobalKeybind(KeybindGlobal, KeybindList const&);

        decltype(m_mKeybinds) const& getAllKeybinds();
        CallbackList const& getCallbacks(KeybindType);
        std::map<std::string, CallbackList> getCallbacksSorted(KeybindType);
        std::string getCategoryName(std::string const&);
        CallbackList getAllCallbacks();
        std::vector<Target> getAllCallbacksForKeybind(Keybind const&);
        CallbackList getCallbacksForKeybind(KeybindType, Keybind const&);
        KeybindList getKeybindsForCallback(KeybindType, KeybindCallback*);
        size_t getIndexOfCallback(KeybindType, KeybindCallback*);
        Target getCallbackByName(std::string const&);
        Target getTargetByID(keybind_id const&);
        void addKeybind(KeybindType, KeybindCallback*, Keybind const&);
        void removeKeybind(KeybindType, KeybindCallback*, Keybind const&);
        void editKeybind(KeybindType, KeybindCallback*, Keybind const& old, Keybind const& rep);
        void clearKeybinds(KeybindType, KeybindCallback*);
        void clearCallbacks(Keybind const&);
        void executeEditorCallbacks(Keybind const&, EditorUI*, bool keydown, bool onlyPlay = false);
        void executePlayCallbacks(Keybind const&, PlayLayer*, bool keydown);
        void resetToDefault(KeybindType, KeybindCallback*, bool = true);
        void resetAllToDefaults();
        void resetUnmodifiedRepeatTimes();
        void handleRepeats(float);
        void registerKeyPress(enumKeyCodes, bool);
        void registerMousePress(MouseButton, bool);
        bool isModifierPressed(keybind_id const&);
        void invokeCallback(keybind_id const&, EditorUI*, PlayLayer*);

        void setDoubleClickInterval(int);
        int getDoubleClickInterval() const;
        bool isAllowedMouseButton(MouseButton btn) const;
        constexpr int getVersion() const;

        static KeybindManager* get();
        static bool initGlobal();
};
