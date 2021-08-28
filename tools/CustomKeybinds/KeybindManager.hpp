#pragma once

#include "../../BetterEdit.hpp"

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

    Keybind();
    Keybind(enumKeyCodes);
    Keybind(enumKeyCodes, Modifiers);
    Keybind(enumKeyCodes, int);
};

namespace std {
    template<>
    struct hash<Keybind> {
        std::size_t operator()(Keybind const&) const;
    };
}

struct KeybindCallback {
    int id;
    std::string name;

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
        decltype(call_b) bind
    ) {
        this->name = keybind;
        this->call_b = bind;
    }

    inline KeybindEditor(
        std::string const& keybind,
        decltype(call) bind
    ) {
        this->name = keybind;
        this->call = bind;
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
        using KeybindList = std::set<Keybind>;
        using CallbackList = std::vector<KeybindCallback*>;
        struct Target {
            KeybindType type;
            KeybindCallback* bind;
        };

    protected:
        std::map<KeybindType, CallbackList> m_mCallbacks;
        std::unordered_map<Keybind, std::vector<Target>> m_mKeybinds;

        bool init();

        void addCallback(KeybindCallback*, KeybindType, KeybindList const&);

    public:
        void encodeDataTo(DS_Dictionary*) override;
        void dataLoaded(DS_Dictionary*) override;
        void firstLoad() override;

        void loadDefaultKeybinds();

        void addEditorKeybind(KeybindEditor, KeybindList const&);
        void addPlayKeybind(KeybindPlayLayer, KeybindList const&);
        void addGlobalKeybind(KeybindGlobal, KeybindList const&);

        CallbackList const& getCallbacks(KeybindType);
        KeybindList getKeybindsForCallback(KeybindType, KeybindCallback*);
        size_t getIndexOfCallback(KeybindType, KeybindCallback*);
        void addKeybind(KeybindType, KeybindCallback*, Keybind const&);
        void removeKeybind(KeybindType, KeybindCallback*, Keybind const&);
        void editKeybind(KeybindType, KeybindCallback*, Keybind const& old, Keybind const& rep);
        void clearKeybinds(KeybindType, KeybindCallback*);
        void executeEditorCallbacks(Keybind const&, EditorUI*, bool keydown);

        static KeybindManager* get();
        static bool initGlobal();
};
