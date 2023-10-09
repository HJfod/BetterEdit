#pragma once

#include "API.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/EditButtonBar.hpp>
#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

struct MoreTabsUI;

namespace editor_api {
    /**
     * Class for adding more tabs to the editor, including the edit and delete menus
     * @class MoreTabs
     */
    class EDITOR_API_EXPORT MoreTabs : public CCNode {
    protected:
        int m_selectedEditTab;
        EditorUI* m_ui;
        CCMenu* m_editTabMenu;
        std::vector<Ref<CCNode>> m_editTabs;

        bool init(EditorUI* ui);
        void onEditTab(CCObject*);
        void updateMode(bool show = true);

        CCMenuItemToggler* createTab(const char* icon, CCObject* target, SEL_MenuHandler selector);

        /**
         * Create 
        */
        static MoreTabs* create(EditorUI* ui);

        friend struct ::MoreTabsUI; 

    public:
        /**
         * Add a new create tab
         * @param icon The sprite frame name of the icon of the tab
         * @param content The content of the tab
         * @returns The tag of the created tab
         */
        int addCreateTab(const char* icon, EditButtonBar* content);

        /**
         * Add a new create tab
         * @param icon The sprite frame name of the icon of the tab
         * @param buttons The list of buttons to add to the EditButtonBar
         * @returns The tag of the created tab
         */
        int addCreateTab(const char* icon, CCArray* buttons);

        /**
         * Add a new create tab
         * @param icon The sprite frame name of the icon of the tab
         * @param objIDs The list of objects to add to the tab
         * @returns The tag of the created tab
         */
        int addCreateTab(const char* icon, std::vector<int> const& objIDs);

        /**
         * Add a new edit tab
         * @param icon The sprite frame name of the icon of the tab
         * @param content The contents of the tab
         * @returns The tag of the created tab
         */
        int addEditTab(const char* icon, CCNode* content);

        /**
         * Get a create tab based on its tag
         * @param tag The tag of the tab
         * @returns The EditButtonBar with the tag, or nullptr
         */
        EditButtonBar* getCreateTab(int tag) const;

        /**
         * Go to a specific edit tab based on its tag
         */
        void switchEditTab(int tag);

        /**
         * Cycle tab selection to the right
         */
        void nextTab();

        /**
         * Cycle tab selection to the left
         */
        void prevTab();

        /**
         * Get the MoreTabs controller for a specific Editor UI
         * @param ui The editor
         * @param create Whether to create the controller if it hasn't been created yet
         * @returns The MoreTabs controller, or nullptr if one doesn't exist
         */
        static MoreTabs* get(EditorUI* ui, bool create = true);
    };
}
