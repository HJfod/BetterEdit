#pragma once

#include "Action.hpp"
#include <BrownAlertDelegate.hpp>
#include "ActionListView.hpp"

enum UndoHistoryMode {
    kUndoHistoryModeGD,
    kUndoHistoryModeHard,
};

class UndoHistoryPopup : public BrownAlertDelegate {
    protected:
        UndoHistoryMode m_eMode = kUndoHistoryModeHard;
        LevelEditorLayer* m_pEditor;
        ActionListView* m_pList = nullptr;
        CCLabelBMFont* m_pInfoLabel;
        std::vector<CCMenuItemToggler*> m_vModeBtns;

        void setup() override;
        void onMode(CCObject*);
        void updateList();
        void updateButtons(CCObject* = nullptr);
    
    public:
        static UndoHistoryPopup* create(LevelEditorLayer*);
};
