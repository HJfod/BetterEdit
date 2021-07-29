#pragma once

#include "../../BetterEdit.hpp"

class RGBColorInputWidget : public CCLayer,
    public TextInputDelegate
{
    protected:
        ColorSelectPopup* parent;
        CCTextInputNode* red_input;
        CCTextInputNode* green_input;
        CCTextInputNode* blue_input;
        CCTextInputNode* hex_input;

        bool init(ColorSelectPopup* parent);

        bool ignore = false; // lmao this is such a hacky fix

        virtual void textChanged(CCTextInputNode* input) override;
    
    public:
        void update_labels(bool hex, bool rgb);

        static RGBColorInputWidget* create(ColorSelectPopup* parent);
};
