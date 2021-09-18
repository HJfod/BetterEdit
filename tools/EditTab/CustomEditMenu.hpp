#pragma once

#include "../../BetterEdit.hpp"
#include "moveForCommand.hpp"

class MoveButtonGroup : public CCObject {
    protected:
        CCArray* m_pButtons;

        bool init();
        virtual ~MoveButtonGroup();

    public:
        static MoveButtonGroup* create();
        void addButton(CCMenuItemSpriteExtra*);
        void setVisible(bool);
};

class CustomEditMenu : public EditButtonBar {
    protected:
        float m_fWidth;
        EditorUI* m_pUI;
        CCMenu* m_pMenu;
        CCArray* m_pMoveGroups;
        CCArray* m_pTransformBtns;
        int m_nMoveGroupPage = 0;

        CCMenuItemSpriteExtra* createButton(
            const char* spr,
            const char* sizeTxt,
            EditCommand com,
            float scale = 1.0f,
            SEL_MenuHandler cb = menu_selector(EditorUI::moveObjectCall),
            const char* keybind = nullptr
        );

        void addMoveGroup(
            EditCommand up,
            EditCommand down,
            EditCommand left,
            EditCommand right,
            const char* sprNum = "",
            const char* sprText = nullptr,
            float sprScale = 1.0f,
            const char* keybind = nullptr
        );

        void addTransformButton(
            EditCommand command,
            const char* spr,
            bool addToDict = false,
            const char* keybind = nullptr,
            SEL_MenuHandler cb = menu_selector(EditorUI::transformObjectCall)
        );

        bool init(EditorUI*);
        
        void onMoveGroupPage(CCObject*);
        void onInfo(CCObject*);

        virtual ~CustomEditMenu();

    public:
        static CustomEditMenu* create(EditorUI*);
};
