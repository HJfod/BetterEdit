#include <Geode/Geode.hpp>
#include <Geode/modify/SetupTriggerPopup.hpp>

using namespace geode::prelude;

class $modify(SetupTriggerPopup) {
    virtual void sliderBegan(Slider* slider) {
        SetupTriggerPopup::sliderBegan(slider);
        if (!Mod::get()->getSettingValue<bool>("hide-trigger-ui")) {
            return;
        }
        getChildOfType<CCScale9Sprite>(m_mainLayer, 0)->runAction(CCFadeTo::create(0.15f, 0));
        this->runAction(CCFadeTo::create(0.15f, 0));
    }
    virtual void sliderEnded(Slider* slider) {
        SetupTriggerPopup::sliderEnded(slider);
        if (!Mod::get()->getSettingValue<bool>("hide-trigger-ui")) {
            return;
        }
        getChildOfType<CCScale9Sprite>(m_mainLayer, 0)->runAction(CCFadeTo::create(0.15f, 255));
        this->runAction(CCFadeTo::create(0.15f, 150));
    }
};
