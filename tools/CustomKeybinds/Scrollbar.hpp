#pragma once

#include "../../BetterEdit.hpp"

class Scrollbar : public CCLayer {
    protected:
        CustomListView* m_pList;
        CCScale9Sprite* m_pBG;
        CCScale9Sprite* m_pTrack;
        CCTouch* m_pTouch;
        float m_fWidth;
        bool m_bScrolling;
    
        bool ccTouchBegan(CCTouch*, CCEvent*) override;
        void ccTouchMoved(CCTouch*, CCEvent*) override;
        void ccTouchEnded(CCTouch*, CCEvent*) override;
        void ccTouchCancelled(CCTouch*, CCEvent*) override;

        void draw() override;
        void visit() override;

        bool init(CustomListView*);

    public:
        void setList(CustomListView* list);

        static Scrollbar* create(CustomListView* list);
};
