#include <Geode/Geode.hpp>
#include <Geode/modify/ColorSelectPopup.hpp>
#include <Geode/modify/SetupPulsePopup.hpp>
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

        if (!Mod::get()->getSettingValue<bool>("rgb-color-input")) {
            return true;
        }

        m_fields->m_alertLayer = static_cast<FLAlertLayer*>(getChildOfType<CCLayer>(this, 0));
        m_fields->m_rgbWidget = RGBInputWidget::create(this, nullptr);
        auto center = CCDirector::sharedDirector()->getWinSize() / 2;
        if (m_isColorTrigger) {
            m_fields->m_rgbWidget->setPosition({center.width - 155.f, center.height + 29.f});
        } else {
            m_fields->m_rgbWidget->setPosition({center.width + 127.f, center.height - 90.f});
        }
        m_fields->m_rgbWidget->setID("rgb-color-input"_spr);
        m_fields->m_alertLayer->addChild(m_fields->m_rgbWidget);

        m_fields->m_rgbWidget->setVisible(!m_copyColor);

        return true;
    }

    void updateColorValue() {
        ColorSelectPopup::updateColorValue();
        if (!Mod::get()->getSettingValue<bool>("rgb-color-input")) {
            return;
        }
        if (m_fields->m_rgbWidget) {
            m_fields->m_rgbWidget->updateLabels(true, true);
        }
    }

    void updateCopyColorTextInputLabel() {
        ColorSelectPopup::updateCopyColorTextInputLabel();
        if (!Mod::get()->getSettingValue<bool>("rgb-color-input")) {
            return;
        }
        if (m_fields->m_rgbWidget) {
            m_fields->m_rgbWidget->setVisible(!m_copyColor);
        }
    }
};

class $modify(SetupPulsePopup) {
    RGBInputWidget* m_rgbWidget = nullptr;
    FLAlertLayer* m_alertLayer = nullptr;
    bool init(EffectGameObject* object, CCArray* objects) {
        if (!SetupPulsePopup::init(object, objects)) {
            return false;
        }

        if (!Mod::get()->getSettingValue<bool>("rgb-color-input")) {
            return true;
        }

        m_fields->m_alertLayer = static_cast<FLAlertLayer*>(getChildOfType<CCLayer>(this, 0));
        m_fields->m_rgbWidget = RGBInputWidget::create(nullptr, this);
        auto center = CCDirector::sharedDirector()->getWinSize() / 2;

        m_colorPicker->setPositionX(m_colorPicker->getPositionX() + 3.7f);
        m_fields->m_rgbWidget->setPosition({ center.width - 132.0f, center.height + 32.0f});

        auto squareWidth = this->m_currentColorSpr->getScaledContentSize().width;
        auto xPos = m_fields->m_rgbWidget->getPositionX() - squareWidth / 2.0f;

        this->m_currentColorSpr->setPosition({ xPos + 20.0f, center.height + 85.0f });
        this->m_prevColorSpr->setPosition({ xPos + 20.0f + squareWidth, center.height + 85.0f });
        m_fields->m_rgbWidget->setID("rgb-color-input"_spr);

        m_fields->m_alertLayer->addChild(m_fields->m_rgbWidget);
        m_fields->m_rgbWidget->setVisible(m_pulseMode == 0);

        return true;
    }

    void updateColorValue() {
        SetupPulsePopup::updateColorValue();
        if (!Mod::get()->getSettingValue<bool>("rgb-color-input")) {
            return;
        }
        if (m_fields->m_rgbWidget) {
            m_fields->m_rgbWidget->updateLabels(true, true);
        }
    }

    void updatePulseMode() {
        SetupPulsePopup::updatePulseMode();
        if (!Mod::get()->getSettingValue<bool>("rgb-color-input")) {
            return;
        }
        if (m_fields->m_rgbWidget) {
            m_fields->m_rgbWidget->setVisible(m_pulseMode == 0);
        }
    }
};