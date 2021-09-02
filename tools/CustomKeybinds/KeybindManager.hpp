#pragma once

#include "../../BetterEdit.hpp"

std::string keyToStringFixed(enumKeyCodes code);

struct Keybind {
    enumKeyCodes key;
    enum Modifiers : int {
        kmNone = 0, kmControl = 1, kmShift = 2, kmAlt = 4,
    };
    int modifiers;
    enum : int {
        kcNone = 0,
        kcLeft, kcRight, kcMiddle,
        kcDoubleClick,
        kcScrollUp, kcScrollDown,
    } click = kcNone;

    bool operator==(Keybind const&) const;
    bool operator<(Keybind const&) const;

    std::string toString() const;
    void save(DS_Dictionary*) const;

    Keybind();
    Keybind(enumKeyCodes);
    Keybind(enumKeyCodes, Modifiers);
    Keybind(enumKeyCodes, int);
    Keybind(DS_Dictionary*);
};

namespace std {
    template<>
    struct hash<Keybind> {
        std::size_t operator()(Keybind const&) const;
    };
}

using KeybindList = std::set<Keybind>;

struct KeybindCallback {
    int id;
    std::string name;
    std::string subcategory = "";
    KeybindList defaults;
    bool repeatable = true;
    bool repeatChanged = false;
    bool repeat = BetterEdit::getKeybindRepeatEnabled();
    int repeatInterval = BetterEdit::getKeybindRepeatInterval();
    int repeatStart = BetterEdit::getKeybindRepeatStart();

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
        decltype(call_b) bind,
        std::string const& cat = ""
    ) {
        this->name = keybind;
        this->call_b = bind;
        this->repeatable = false;
        this->subcategory = cat;
    }
    
    inline KeybindEditor(
        std::string const& keybind,
        decltype(call) bind,
        std::string const& cat = "",
        bool isRepeatable = true
    ) {
        this->name = keybind;
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
        this->call = copy.call;
        this->call_e = copy.call_e;
        this->id = copy.id;
        this->editor = alsoInEditor;
        this->repeatable = copy.repeatable;
    }

    inline KeybindPlayLayer(
        std::string const& keybind,
        decltype(call) bind
    ) {
        this->name = keybind;
        this->call = bind;
        this->editor = false;
    }

    inline KeybindPlayLayer(
        std::string const& keybind,
        decltype(call_e) bind
    ) {
        this->name = keybind;
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
            std::unordered_map<std::string, KeybindList>
        > m_mLoadedBinds;
        std::unordered_map<enumKeyCodes, float> m_mHeldKeys;

        bool init();

        void addCallback(KeybindCallback*, KeybindType, KeybindList const&);
        KeybindList getLoadedBinds(KeybindType, KeybindCallback*, bool*);

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
        CallbackList getCallbacksForKeybind(KeybindType, Keybind const&);
        KeybindList getKeybindsForCallback(KeybindType, KeybindCallback*);
        size_t getIndexOfCallback(KeybindType, KeybindCallback*);
        Target getCallbackByName(std::string const&);
        void addKeybind(KeybindType, KeybindCallback*, Keybind const&);
        void removeKeybind(KeybindType, KeybindCallback*, Keybind const&);
        void editKeybind(KeybindType, KeybindCallback*, Keybind const& old, Keybind const& rep);
        void clearKeybinds(KeybindType, KeybindCallback*);
        void executeEditorCallbacks(Keybind const&, EditorUI*, bool keydown, bool onlyPlay = false);
        void executePlayCallbacks(Keybind const&, PlayLayer*, bool keydown);
        void resetToDefault(KeybindType, KeybindCallback*, bool = true);
        void resetAllToDefaults();
        void resetUnmodifiedRepeatTimes();
        void handleRepeats(float);
        void registerKeyPress(enumKeyCodes, bool);

        static KeybindManager* get();
        static bool initGlobal();
};
