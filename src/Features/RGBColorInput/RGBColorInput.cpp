#include <Geode/Geode.hpp>
#include <Geode/modify/ColorSelectPopup.hpp>
#include <Geode/utils/cocos.hpp>

#include "RGBInputWidget.hpp"

using namespace geode::prelude;

class $modify(ColorSelectPopup) {
    FLAlertLayer* m_alertLayer = nullptr;
    RGBInputWidget* m_rgbWidget = nullptr;

    bool init(EffectGameObject* object, CCArray* objects, ColorAction* colorAction) {
        if (!ColorSelectPopup::init(object, objects, colorAction)) {
            return false;
        }

        m_fields->m_alertLayer = static_cast<FLAlertLayer*>(getChildOfType<CCLayer>(this, 0));
        m_fields->m_rgbWidget = RGBInputWidget::create(this, nullptr);
        auto center = CCDirector::sharedDirector()->getWinSize() / 2;
        m_fields->m_rgbWidget->setPosition(center.width + 100.f, center.height - 20.f);
        if (m_isColorTrigger) {
            m_fields->m_rgbWidget->setPosition({center.width - 155.f, center.height + 29.f});
        } else {
            m_fields->m_rgbWidget->setPosition({center.width + 127.f, center.height - 90.f});
        }
        m_fields->m_alertLayer->addChild(m_fields->m_rgbWidget);

        m_fields->m_rgbWidget->setVisible(!m_copyColor);

        return true;
    }

    void updateColorValue() {
        ColorSelectPopup::updateColorValue();
        if (m_fields->m_rgbWidget) {
            m_fields->m_rgbWidget->updateLabels(true, true);
        }
    }

    void updateCopyColorTextInputLabel() {
        ColorSelectPopup::updateCopyColorTextInputLabel();
        if (m_fields->m_rgbWidget) {
            m_fields->m_rgbWidget->setVisible(!m_copyColor);
        }
    }
};