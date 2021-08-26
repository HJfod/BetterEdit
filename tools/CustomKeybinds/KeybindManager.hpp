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
    bool(__fastcall* call)(EditorUI*);

    inline KeybindEditor(
        std::string const& keybind,
        bool(__fastcall* bind)(EditorUI*)
    ) {
        this->name = keybind;
        this->call = bind;
    }
};

struct KeybindPlayLayer : public KeybindCallback {
    std::function<void(PlayLayer*)> call;

    inline KeybindPlayLayer(
        std::string const& keybind,
        std::function<void(PlayLayer*)> bind
    ) {
        this->name = keybind;
        this->call = bind;
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
            size_t index;
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
        KeybindList getKeybindsForCallback(KeybindType, size_t ix);
        size_t getIndexOfCallback(KeybindType, KeybindCallback*);
        void addKeybind(KeybindType, size_t cbIx, Keybind const&);
        void clearKeybinds(KeybindType, size_t cbIx);
        void executeEditorCallbacks(Keybind const&, EditorUI*);

        static KeybindManager* get();
        static bool initGlobal();
};
