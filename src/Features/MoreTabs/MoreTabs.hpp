#pragma once

#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/EditButtonBar.hpp>
#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

struct MoreTabsUI;

class MoreTabs : public CCNode {
protected:
    int m_selectedEditTab;
    EditorUI* m_ui;
    CCMenu* m_editTabMenu;
    std::vector<Ref<CCNode>> m_editTabs;

    bool init(EditorUI* ui);
    void onEditTab(CCObject*);
    void updateMode(bool show = true);

    CCMenuItemToggler* createTab(const char* icon, CCObject* target, SEL_MenuHandler selector);

    friend struct MoreTabsUI; 

public:
    int addCreateTab(const char* icon, EditButtonBar* content);
    int addCreateTab(const char* icon, CCArray* buttons);
    int addCreateTab(const char* icon, std::vector<int> const& objIDs);
    int addEditTab(const char* icon, CCNode* content);

    EditButtonBar* getCreateTab(int tag) const;
    void switchEditTab(int tag);

    void nextTab();
    void prevTab();

    static MoreTabs* create(EditorUI* ui);
    static MoreTabs* get(EditorUI* ui, bool create = true);
};
