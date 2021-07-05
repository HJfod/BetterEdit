#pragma once

#include "../../BetterEdit.hpp"
#include <BrownAlertDelegate.hpp>

class PasteLayer : public BrownAlertDelegate {
    public:
        enum State {
            ObjID, Groups, ZLayer, ZOrder, EditorLayer, EditorLayer2,
            DontFade, DontEnter, PositionX, PositionY, Rotation, Scale,
            HighDetail,
        };

    protected:
        std::vector<gd::CCMenuItemToggler*> m_vToggles;
        int m_nToggleCount;
        bool m_bNextRow = false;
        
        void setup() override;

        void addStateToggle(const char* text, State state);

        void onToggle(cocos2d::CCObject*);
        void onToggleAll(cocos2d::CCObject*);
        void onPasteState(cocos2d::CCObject*);
    
    public:
        static PasteLayer* create();

        static std::set<State> * getStates();
        static bool wantsToPasteState();
};
