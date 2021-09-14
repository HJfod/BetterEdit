#pragma once

#include "../../BetterEdit.hpp"
#include "../CustomKeybinds/KeybindManager.hpp"

struct EditorButtonItem {
    const char* spriteName;
};

using EditorButtonMap = std::unordered_map<keybind_id, EditorButtonItem>;

EditorButtonMap const& getEditorButtons();

class EditorButton : public CCMenuItemSpriteExtra {
    protected:
        keybind_id m_sID;
        EditorButtonItem m_obItem;
        EditorUI* m_pUI;

        bool initWithID(EditorUI*, keybind_id const&);

    public:
        static EditorButton* createFromID(EditorUI*, keybind_id const&);

        void activate() override;
        void setEnabled(bool) override;
};
