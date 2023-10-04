#include <Geode/Geode.hpp>
#include <Geode/modify/GJScaleControl.hpp>
#include <Geode/modify/EditorUI.hpp>

#include "InputScaleDelegate.hpp"

using namespace geode::prelude;

class $modify(BetterScaleControl, GJScaleControl) {
    CCTextInputNode* m_textInput = nullptr;

    bool init() {
        if (!GJScaleControl::init()) {
            return false;
        }

        m_label->setVisible(false);

        auto sprite = CCScale9Sprite::create(
            "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
        );

        sprite->setScale(0.3f);
        sprite->setColor({ 0, 0, 0 });
        sprite->setOpacity(100);
        sprite->setContentSize({ 115.0f, 75.0f });
        sprite->setPosition(m_label->getPosition());
        sprite->setID("scale-input-sprite"_spr);

        auto input = CCTextInputNode::create(40.0f, 30.0f, "1.00", "bigFont.fnt");
        input->setPosition(m_label->getPosition());
        input->ignoreAnchorPointForPosition(false);
        input->setLabelPlaceholderColor({ 120, 120, 120 });
        input->setAllowedChars(".0123456789");
        input->m_textField->setAnchorPoint({ 0.5f, 0.5f });
        input->m_placeholderLabel->setAnchorPoint({ 0.5f, 0.5f });
        input->setScale(0.7f);
        input->setID("scale-input"_spr);
        input->setMouseEnabled(true);
        input->setTouchEnabled(true);
        m_fields->m_textInput = input;

        auto delegate = InputScaleDelegate::create(this);
        input->setDelegate(delegate);

        this->addChild(sprite);
        this->addChild(input);

        return true;
    }

    void loadValues(GameObject* object, CCArray* objects) {
        GJScaleControl::loadValues(object, objects);

        if (object) {
            log::info("object scale: {}", object->getScale());
        }
        float value = m_slider->m_touchLogic->m_thumb->getValue();
        auto scale = 1.5f * value + 0.5f;
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << scale;

        std::string s = ss.str();

        if (m_fields->m_textInput) {
            m_fields->m_textInput->setString(s.c_str());
        }
    }

    void updateLabel(float value) {
        GJScaleControl::updateLabel(value);

        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << m_value;

        std::string s = ss.str();

        if (m_fields->m_textInput) {
            m_fields->m_textInput->setString(s.c_str());
        }
    }
};

class $modify(ScaleEditorUI, EditorUI) {
    void upadteSpecialUIElements() {
        EditorUI::updateSpecialUIElements();

        auto text = static_cast<CCTextInputNode*>(m_scaleControl->getChildByID("scale-input"_spr));
        if (text) {
            text->attachWithIME();
        }
    }

    void activateScaleControl(CCObject* sender) {
        EditorUI::activateScaleControl(sender);
        auto pos = CCDirector::sharedDirector()->getWinSize() / 2;
        pos.height += 50.0f;
        pos = m_editorLayer->m_objectLayer->convertToNodeSpace(pos);
        m_scaleControl->setPosition(pos);
    }

    bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
        if (this->touchedScaleInput(touch, event)) {
            return true;
        }
        return EditorUI::ccTouchBegan(touch, event);
    }

    bool touchedScaleInput(CCTouch* touch, CCEvent* event) {
        auto input = static_cast<CCTextInputNode*>(this->m_scaleControl->getChildByID("scale-input"_spr));
        if (!m_scaleControl->isVisible()) {
            if (input) {
                input->getTextField()->detachWithIME();
            }
            return false;
        }

        if (!input) {
            return false;
        }

        auto nodeSize = input->getScaledContentSize();
        nodeSize.width *= 2.0f;
        nodeSize.height *= 1.5f;

        auto inputRect = CCRect {
            m_scaleControl->getPositionX() + input->getPositionX() - nodeSize.width / 2,
            (m_scaleControl->getPositionY() + input->getPositionY() - nodeSize.height / 2) + 12.5f,
            nodeSize.width,
            nodeSize.height
        };

        auto pos = GameManager::sharedState()
            ->getEditorLayer()
            ->getObjectLayer()
            ->convertTouchToNodeSpace(touch);
        
        if (inputRect.containsPoint(pos)) {
            input->getTextField()->attachWithIME();
            return true;
        } 

        input->getTextField()->detachWithIME();
        return false;
    }
};
