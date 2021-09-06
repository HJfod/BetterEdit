#pragma once

#include "ContextPopup.hpp"
#include "KeybindListView.hpp"

class MoreKeybindsPopup : public ContextPopup {
    protected:
        KeybindCell* m_pCell;

        void setup() override;
    
    public:
        static MoreKeybindsPopup* create(KeybindCell*, CCMenuItemSpriteExtra*);
};
