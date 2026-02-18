#pragma once

#include <features/supporters/Pro.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>

using namespace geode::prelude;

enum class GroupSummaryFilter {
    All,
    Used,
    Unused,
    NoTriggers,
    NoTargets,
};
enum class GroupSummaryView {
    List,
    Grid,
};

struct ObjectsInGroupID final {
    int groupID;
    Ref<CCArray> targets;
    Ref<CCArray> triggers;
};
struct Pages final {
    std::vector<std::vector<ObjectsInGroupID>> pages;
    size_t totalMatches;

    inline void reset() {
        this->pages.clear();
        this->totalMatches = 0;
    }
};

class GroupSummaryPopup;

class ObjectsListPopup : public Popup {
protected:
    GroupSummaryPopup* m_popup;
    Ref<CCArray> m_objs;

    bool init(std::string const& title, GroupSummaryPopup* popup, CCArray* objects, bool render);

    void onSelect(CCObject*);

public:
    static ObjectsListPopup* create(std::string const& title, GroupSummaryPopup* popup, CCArray* objects, bool render);
};

class GroupSummaryPopup : public Popup {
protected:
    EditorUI* m_ui;
    CCNode* m_groupsListContainer;
    CCMenu* m_viewMenu;
    CCMenu* m_filtersMenu;
    std::map<int, ObjectsInGroupID> m_objectsInGroupsCache;
    Pages m_pages;
    size_t m_page;
    GroupSummaryView m_view;
    GroupSummaryFilter m_filter;
    CCMenuItemSpriteExtra* m_prevPageBtn;
    CCMenuItemSpriteExtra* m_nextPageBtn;
    CCLabelBMFont* m_pageLabel;
    CCLabelBMFont* m_matchesLabel;
    TextInput* m_search;

    friend class ObjectsListPopup;

    bool init(EditorUI* ui);

    void loadPage(size_t page);
    void updatePages(size_t page = 0);
    void onPage(CCObject* sender);
    void onFilter(CCObject* sender);
    void onView(CCObject* sender);

    void onGroupTriggers(CCObject* sender);
    void onGroupTargets(CCObject* sender);

public:
    static GroupSummaryPopup* create(EditorUI* ui);
};
