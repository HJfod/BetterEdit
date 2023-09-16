
#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/Slider.hpp>
#include <Geode/binding/SliderThumb.hpp>
#include <Geode/binding/SliderTouchLogic.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

class $modify(SliderUI, EditorUI) {
    void updateSlider() {
        EditorUI::updateSlider();
        this->updatePercentage();
    }

    void updateZoom(float zoom) {
        EditorUI::updateZoom(zoom);
        this->updatePercentage();
    }

    void sliderChanged(CCObject* slider) {
        EditorUI::sliderChanged(slider);
        this->updatePercentage();
    }
    
    void scrollWheel(float y, float x) override {
        EditorUI::scrollWheel(y, x);
        this->updatePercentage();
    }

    void updatePercentage() {
        if (!Mod::get()->template getSettingValue<bool>("show-percentage")) {
            return;
        }
        // only update once per frame
        if (!this->getActionByTag(0xBE77E)) {
            auto action = CCSequence::create(
                CCDelayTime::create(0.f),
                CCCallFunc::create(this, callfunc_selector(SliderUI::doUpdatePercentage)),
                nullptr
            );
            action->setTag(0xBE77E);
            this->runAction(action);
        }
    }

    void doUpdatePercentage() {
        auto perLabel = static_cast<CCLabelBMFont*>(
            m_positionSlider->getChildByID("percentage-label"_spr)
        );
        if (!perLabel) {
            perLabel = CCLabelBMFont::create("", "bigFont.fnt");
            perLabel->setID("percentage-label"_spr);
            perLabel->setScale(.35f);
            m_positionSlider->addChild(perLabel);
        }   
        auto thumbPos = m_positionSlider->m_touchLogic->m_thumb->getPosition();

        // C++20 doesn't allow dynamic format str
        //perLabel->setString(fmt::format(Mod::get()->getSettingValue<bool>("use-time-for-percentage") ? "{:.0f} sec" : "{:.0f}%", calcPercentage()).c_str());
        auto percStr = Mod::get()->getSettingValue<bool>("use-time-for-percentage") ? fmt::format("{:.0f} sec", calcPercentage()) : fmt::format("{:.0f}%", calcPercentage());
        perLabel->setString(percStr.c_str());

        perLabel->setPosition(thumbPos.x, thumbPos.y - 25.f);
        perLabel->setOpacity(255);
        perLabel->stopAllActions();
        perLabel->runAction(CCSequence::create(
            CCDelayTime::create(.5f),
            CCFadeOut::create(.5f),
            nullptr
        ));
    }

    float timeForXPos(float xPos) {
        return reinterpret_cast<float(__vectorcall*)(LevelEditorLayer*, float, float)>(
            geode::base::get() + 0x9c7d0
            )(LevelEditorLayer::get(), 1907.0f, xPos);
    }

    float calcPercentage() {
        float max = 0.f;
        for (auto obj : CCArrayExt<GameObject>(m_editorLayer->m_objects)) {
            if (obj->getPositionX() > max) {
                max = obj->getPositionX();
            }
        }
        if (max == 0.f) return 0.f;
        auto winSize = CCDirector::get()->getWinSize();
        auto pos = m_editorLayer->m_objectLayer->convertToNodeSpace(winSize / 2);
        
        if (Mod::get()->getSettingValue<bool>("use-time-for-percentage"))
            return m_editorLayer->timeForXPos(pos.x); // it's void in the bindings lol change it to float

        return pos.x / max * 100;
    }
};
