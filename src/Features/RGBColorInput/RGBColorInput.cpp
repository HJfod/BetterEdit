#include <Geode/Geode.hpp>
#include <Geode/modify/ColorSelectPopup.hpp>
#include <Geode/modify/SetupPulsePopup.hpp>
#include <Geode/utils/cocos.hpp>

#include "RGBInputWidget.hpp"

using namespace geode::prelude;

// Credit to mat for creating this
// https://github.com/matcool/geode-mods/blob/main/rgb-color-input/

class $modify(ColorSelectPopup) {
    FLAlertLayer* alertLayer = nullptr;
    RGBInputWidget* rgbWidget = nullptr;

    bool init(EffectGameObject* object, CCArray* objects, ColorAction* colorAction) {
        if (!ColorSelectPopup::init(object, objects, colorAction)) {
            return false;
        }

        if (!Mod::get()->getSettingValue<bool>("rgb-color-input")) {
            return true;
        }

        m_fields->alertLayer = static_cast<FLAlertLayer*>(getChildOfType<CCLayer>(this, 0));
        m_fields->rgbWidget = RGBInputWidget::create(this, nullptr);
        auto center = CCDirector::sharedDirector()->getWinSize() / 2;
        if (m_isColorTrigger) {
            m_fields->rgbWidget->setPosition({center.width - 155.f, center.height + 29.f});
        } else {
            m_fields->rgbWidget->setPosition({center.width + 127.f, center.height - 90.f});
        }
        m_fields->rgbWidget->setID("rgb-color-input"_spr);
        m_fields->alertLayer->addChild(m_fields->rgbWidget);

        m_fields->rgbWidget->setVisible(!m_copyColor);

        return true;
    }

    void colorValueChanged(ccColor3B color) {
        ColorSelectPopup::colorValueChanged(color);
        if (!Mod::get()->getSettingValue<bool>("rgb-color-input")) {
            return;
        }
        if (m_fields->rgbWidget) {
            m_fields->rgbWidget->updateLabels(true, true);
        }
    }

    void updateCopyColorTextInputLabel() {
        ColorSelectPopup::updateCopyColorTextInputLabel();
        if (!Mod::get()->getSettingValue<bool>("rgb-color-input")) {
            return;
        }
        if (m_fields->rgbWidget) {
            m_fields->rgbWidget->setVisible(!m_copyColor);
        }
    }
};

class $modify(SetupPulsePopup) {
    RGBInputWidget* rgbWidget = nullptr;
    FLAlertLayer* alertLayer = nullptr;
    bool init(EffectGameObject* object, CCArray* objects) {
        if (!SetupPulsePopup::init(object, objects)) {
            return false;
        }

        if (!Mod::get()->getSettingValue<bool>("rgb-color-input")) {
            return true;
        }

        m_fields->alertLayer = static_cast<FLAlertLayer*>(getChildOfType<CCLayer>(this, 0));
        m_fields->rgbWidget = RGBInputWidget::create(nullptr, this);
        auto center = CCDirector::sharedDirector()->getWinSize() / 2;

        m_colorPicker->setPositionX(m_colorPicker->getPositionX() + 3.7f);
        m_fields->rgbWidget->setPosition({ center.width - 132.0f, center.height + 32.0f});

        auto squareWidth = m_currentColorSpr->getScaledContentSize().width;
        auto xPos = m_fields->rgbWidget->getPositionX() - squareWidth / 2.0f;

        m_currentColorSpr->setPosition({ xPos + 20.0f, center.height + 85.0f });
        m_prevColorSpr->setPosition({ xPos + 20.0f + squareWidth, center.height + 85.0f });
        m_fields->rgbWidget->setID("rgb-color-input"_spr);

        m_fields->alertLayer->addChild(m_fields->rgbWidget);
        m_fields->rgbWidget->setVisible(m_pulseMode == 0);

        return true;
    }

    void colorValueChanged(ccColor3B color) {
        SetupPulsePopup::colorValueChanged(color);
        if (!Mod::get()->getSettingValue<bool>("rgb-color-input")) {
            return;
        }
        if (m_fields->rgbWidget) {
            m_fields->rgbWidget->updateLabels(true, true);
        }
    }

    void updatePulseMode() {
        SetupPulsePopup::updatePulseMode();
        if (!Mod::get()->getSettingValue<bool>("rgb-color-input")) {
            return;
        }
        if (m_fields->rgbWidget) {
            m_fields->rgbWidget->setVisible(m_pulseMode == 0);
        }
    }
};