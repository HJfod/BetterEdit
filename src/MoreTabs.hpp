#pragma once

#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/EditButtonBar.hpp>
#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

struct MoreTabsUI;

class MoreTabs : public CCNode {
protected:
    int m_selected;
    EditorUI* m_ui;
    CCMenu* m_editTabMenu;
    std::vector<Ref<CCNode>> m_editTabs;

    bool init(EditorUI* ui);
    void onEditTab(CCObject*);
    void updateMode(bool show = true);

    CCMenuItemToggler* createTab(const char* icon, CCObject* target, SEL_MenuHandler selector);

    friend struct MoreTabsUI; 

public:
    void addCreateTab(const char* icon, std::vector<int> const& objIDs);
    void addEditTab(const char* icon, CCNode* content);

    static MoreTabs* create(EditorUI* ui);
    static MoreTabs* get(EditorUI* ui, bool create = true);
};
