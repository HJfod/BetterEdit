#pragma once

#include "../../BetterEdit.hpp"
#include <BrownAlertDelegate.hpp>
#include "../CustomKeybinds/SuperMouseManager.hpp"
#include "../CustomKeybinds/ContextPopup.hpp"

struct GroupSummary {
    std::vector<GameObject*> m_vObjects;
    std::vector<EffectGameObject*> m_vTriggers;
};

class MoreTriggersPopup;
class GroupListView;

class GroupSummaryPopup :
    public BrownAlertDelegate
{
    public:
        enum Show {
            kShowAll,
            kShowOnlyOnesWithSomething,
            kShowOnesWithObjectsButNoTriggers,
            kShowOnesWithTriggersButNoObjects,
        };

    protected:
        LevelEditorLayer* m_pEditor;
        GroupListView* m_pList = nullptr;
        std::vector<CCNode*> m_vPageContent;
        int m_nPage = 0;
        int m_nItemCount = 20;
        float m_fItemWidth = 400.f;
        std::unordered_map<int, GroupSummary> m_mGroupInfo;
        std::vector<int> m_vShowArray;
        Show m_eShow = kShowAll;
        std::vector<CCMenuItemToggler*> m_vShowBtns;
        CCLabelBMFont* m_pNoFilterInfo;
        CCLabelBMFont* m_pGroupCount;

        void setup() override;
        void onShow(CCObject*);
        void onPage(CCObject*);
        void incrementPage(int);
        void goToPage(int);
        void updatePage();
        void updateGroups();
        void updateFilters(CCObject* = nullptr);
        void addGroups(GameObject* obj);
        void addGroups(EffectGameObject* obj);
        void keyDown(enumKeyCodes) override;

        virtual ~GroupSummaryPopup();
        
        friend class MoreTriggersPopup;

    public:
        decltype(m_mGroupInfo) & getGroupInfo();
        GroupSummary & getGroup(int);

        static GroupSummaryPopup* create(LevelEditorLayer*);
};
