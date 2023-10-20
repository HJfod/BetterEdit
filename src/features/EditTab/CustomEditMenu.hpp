#pragma once

#include "Geode/modify/EditButtonBar.hpp"
#include "Geode/modify/EditorUI.hpp"

#include "Geode/Utils.hpp"
#include "geode.custom-keybinds/include/Keybinds.hpp"

using namespace geode::prelude;

#include "MoveForCommand.hpp"

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

// class CustomEditMenu : public EditButtonBar {
//     protected:
        

//         CCMenuItemSpriteExtra* createButton(
//             const char* spr,
//             const char* sizeTxt,
//             CustomEditCommand com,
//             float scale = 1.0f,
//             SEL_MenuHandler cb = menu_selector(EditorUI::moveObjectCall),
//             const char* keybind = nullptr
//         );

//         void addMoveGroup(
//             CustomEditCommand up,
//             CustomEditCommand down,
//             CustomEditCommand left,
//             CustomEditCommand right,
//             const char* sprNum = "",
//             const char* sprText = nullptr,
//             float sprScale = 1.0f,
//             const char* keybind = nullptr
//         );

//         void addTransformButton(
//             CustomEditCommand command,
//             const char* spr,
//             bool addToDict = false,
//             const char* keybind = nullptr,
//             SEL_MenuHandler cb = menu_selector(EditorUI::transformObjectCall)
//         );

//         bool init(EditorUI*);
        
//         void onMoveGroupPage(CCObject*);
//         void onInfo(CCObject*);

//         CustomEditMenu() : EditButtonBar(ZeroConstructorType {}) {};
//         virtual ~CustomEditMenu();

//     public:
//         static CustomEditMenu* create(EditorUI*);
// };