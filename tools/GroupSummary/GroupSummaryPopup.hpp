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

class GroupSummaryPopup : public BrownAlertDelegate {
    protected:
        LevelEditorLayer* m_pEditor;
        std::vector<CCNode*> m_vPageContent;
        int m_nPage = 0;
        int m_nItemCount = 7;
        float m_fItemWidth = 400.f;
        std::unordered_map<int, GroupSummary> m_mGroupInfo;

        void setup() override;
        void onPage(CCObject*);
        void onShowRestOfTheTriggers(CCObject*);
        void updatePage();
        void updateGroups();
        void addGroups(GameObject* obj);
        void addGroups(EffectGameObject* obj);
        CCNode* createItem(int);

        virtual ~GroupSummaryPopup();
        
        friend class MoreTriggersPopup;

    public:
        static GroupSummaryPopup* create(LevelEditorLayer*);
};
