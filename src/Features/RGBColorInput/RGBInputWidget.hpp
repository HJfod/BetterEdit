#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class RGBInputWidget : public CCLayer, public TextInputDelegate {
protected:
    ColorSelectPopup* m_colorSelect = nullptr;
    SetupPulsePopup* m_setupPulse = nullptr;
    CCTextInputNode* m_redInput = nullptr;
    CCTextInputNode* m_greenInput = nullptr;
    CCTextInputNode* m_blueInput = nullptr;
    CCTextInputNode* m_hexInput = nullptr;

    // so, I update the color, that updates my text inputs, that updates the color, that updates my text inputs, that updates the color, tha-
    bool m_ignore = false;

    bool init(ColorSelectPopup* colorSelect, SetupPulsePopup* setupPulse);

    ccColor3B getParentColor();
    void setParentColor(ccColor3B color);
    inline std::string colorToHex(ccColor3B color);
    virtual void textChanged(CCTextInputNode* input) override;

    Result<ccColor3B> getColorFromHex();
    ccColor3B getColorFromRGB(CCTextInputNode* input);
public:
    void updateLabels(bool hex, bool rgb);
    // Call with one of the those as parents, leave the other as nullptr
    static RGBInputWidget* create(ColorSelectPopup* colorSelect, SetupPulsePopup* setupPulse) {
        auto ret = new RGBInputWidget;
        if (ret && ret->init(colorSelect, setupPulse)) {
            ret->autorelease();
            return ret;
        }

        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};