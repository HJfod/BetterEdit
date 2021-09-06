#pragma once

#include "../../BetterEdit.hpp"

enum ContextPopupDirection {
    kContextPopupDirectionLeft,
    kContextPopupDirectionRight,
    kContextPopupDirectionUp,
    kContextPopupDirectionDown,
};

class ContextPopup : public CCLayer {
    protected:
        ContextPopupDirection m_eDirection;
        CCSize m_obSize;
        CCScale9Sprite* m_pBG;

        virtual void setup() = 0;

        bool init(
            CCPoint const&,
            CCSize const&,
            ContextPopupDirection
        );
};
