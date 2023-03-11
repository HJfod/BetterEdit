
#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/Slider.hpp>
#include <Geode/binding/SliderThumb.hpp>
#include <Geode/binding/SliderTouchLogic.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

class $modify(EditorUI) {
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
        if (!Mod::get()->template getSettingValue<bool>("show-zoom-text")) {
            return;
        }
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
        perLabel->setString(fmt::format("{:.0f}%", calcPercentage()).c_str());
        perLabel->setPosition(thumbPos.x, thumbPos.y - 25.f);
        perLabel->setOpacity(255);
        perLabel->stopAllActions();
        perLabel->runAction(CCSequence::create(
            CCDelayTime::create(.5f),
            CCFadeOut::create(.5f),
            nullptr
        ));
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
        return pos.x / max * 100;
    }
};
