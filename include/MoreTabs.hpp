#pragma once

#include "Macros.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/EditButtonBar.hpp>
#include <Geode/utils/cocos.hpp>

struct MoreTabsUI;

namespace better_edit {
    class BE_DLL MoreTabs : public cocos2d::CCNode {
    protected:
        int m_selectedEditTab;
        EditorUI* m_ui;
        cocos2d::CCMenu* m_editTabMenu;
        std::vector<geode::Ref<cocos2d::CCNode>> m_editTabs;

        bool init(EditorUI* ui);
        void onEditTab(cocos2d::CCObject*);
        void updateMode(bool show = true);

        CCMenuItemToggler* createTab(const char* icon, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler selector);

        friend struct ::MoreTabsUI; 

    public:
        int addCreateTab(const char* icon, EditButtonBar* content);
        int addCreateTab(const char* icon, cocos2d::CCArray* buttons);
        int addCreateTab(const char* icon, std::vector<int> const& objIDs);
        int addEditTab(const char* icon, cocos2d::CCNode* content);

        EditButtonBar* getCreateTab(int tag) const;

        void nextTab();
        void prevTab();

        static MoreTabs* create(EditorUI* ui);
        static MoreTabs* get(EditorUI* ui, bool create = true);
    };
}
