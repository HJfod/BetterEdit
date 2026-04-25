#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/EffectGameObject.hpp>
#include <Geode/binding/GJEffectManager.hpp>
#include <Geode/binding/PlayerObject.hpp>
#include <Geode/binding/LevelTools.hpp>
#include <Geode/binding/DrawGridLayer.hpp>
#include <Geode/binding/LevelSettingsObject.hpp>
#include <Geode/binding/GJGroundLayer.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <utils/Editor.hpp>

using namespace geode::prelude;

class $modify(TriggerPreviewUI, EditorUI) {
    struct Fields final {
        std::vector<Ref<EffectGameObject>> preview;
        std::pair<float, float> previewRange;
        CCMenuItemToggler* previewToggle = nullptr;
        CCMenuItemSpriteExtra* previewKnob = nullptr;
        ListenerHandle onUIHide;
    };

    $override
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;

        if (!Mod::get()->getSettingValue<bool>("trigger-previews")) {
            return true;
        }

        if (auto menu = this->querySelector("editor-buttons-menu")) {
            auto sprOff = EditorButtonSprite::createWithSpriteFrameName("preview-trigger.png"_spr, .85f, EditorBaseColor::Orange);
            auto sprOn = EditorButtonSprite::createWithSpriteFrameName("preview-trigger-stop.png"_spr, .85f, EditorBaseColor::Pink);
            sprOff->setContentSize(ccp(40, 40));
            sprOn->setContentSize(ccp(40, 40));
            m_fields->previewToggle = CCMenuItemToggler::create(sprOff, sprOn, this, menu_selector(TriggerPreviewUI::onPreviewTrigger));
            m_fields->previewToggle->setID("preview-trigger"_spr);
            m_fields->previewToggle->m_notClickable = true;
            menu->addChild(m_fields->previewToggle);
            menu->updateLayout();

            m_fields->onUIHide = UIShowEvent(this).listen([this](bool show) {
                const bool PREVIEWING = !m_fields->preview.empty();
                m_fields->previewToggle->setVisible(PREVIEWING || show);
            });

            this->schedule(schedule_selector(TriggerPreviewUI::onPreviewFrame));
            this->updateButtons();
        }
        
        return true;
    }

    $override
    void updateButtons() {
        EditorUI::updateButtons();
        if (m_fields->previewToggle) {
            bool hasTriggerSelected = false;
            for (auto obj : be::getSelectedObjects(this)) {
                if (typeinfo_cast<EffectGameObject*>(obj)) {
                    hasTriggerSelected = true;
                }
            }
            be::enableToggle(m_fields->previewToggle, hasTriggerSelected);
        }
    }

    void onPreviewTrigger(CCObject*) {
        if (m_fields->preview.empty()) {
            this->startPreview();
        }
        else {
            this->stopPreview();
        }
    }

    void startPreview() {
        this->stopPreview();

        m_fields->previewToggle->toggle(true);

        m_fields->previewKnob = CCMenuItemSpriteExtra::create(
            CCSprite::create("sliderthumb.png"),
            this, nullptr
        );
        m_fields->previewKnob->setZOrder(9999);
        m_editorLayer->m_objectLayer->addChild(m_fields->previewKnob);

        float lowest = std::numeric_limits<float>::infinity();

        bool first = true;
        for (auto obj : be::getSelectedObjects(this)) {
            if (auto trigger = typeinfo_cast<EffectGameObject*>(obj)) {
                m_fields->preview.push_back(trigger);
                trigger->triggerObject(m_editorLayer, 1, nullptr);

                // Calculate the span of the level the trigger takes up
                auto time = LevelTools::timeForPos(
                    trigger->getPosition(),
                    m_editorLayer->m_drawGridLayer->m_speedObjects,
                    static_cast<int>(m_editorLayer->m_levelSettings->m_startSpeed),
                    trigger->m_ordValue,
                    trigger->m_channelValue,
                    false,
                    m_editorLayer->m_levelSettings->m_platformerMode,
                    true,
                    false,
                    false
                );
                auto targetPos = LevelTools::posForTimeInternal(
                    time + trigger->m_duration,
                    m_editorLayer->m_drawGridLayer->m_speedObjects,
                    static_cast<int>(m_editorLayer->m_levelSettings->m_startSpeed),
                    m_editorLayer->m_levelSettings->m_platformerMode,
                    false,
                    true,
                    m_editorLayer->m_gameState.m_rotateChannel,
                    0
                );

                if (first || trigger->getPositionX() < m_fields->previewRange.first) {
                    m_fields->previewRange.first = trigger->getPositionX();
                }
                if (first || targetPos.x > m_fields->previewRange.second) {
                    m_fields->previewRange.second = targetPos.x;
                }
                if (trigger->getPositionY() < lowest) {
                    lowest = trigger->getPositionY();
                }
                
                first = false;
            }
        }
        // todo: if firstX == lastX still show but snappy boolean before/after 
        m_fields->previewKnob->setPosition(ccp(m_fields->previewRange.first, lowest - 30));
    }
    void stopPreview() {
        // todo: denna kunde vara bättre
        m_editorLayer->onStopPlaytest();

        if (m_fields->previewKnob) {
            m_fields->previewKnob->removeFromParent();
            m_fields->previewKnob = nullptr;
        }
        m_fields->previewToggle->toggle(false);
        m_fields->preview.clear();

        this->showUI(true);
    }

    void onPreviewFrame(float dt) {
        if (m_fields->preview.empty()) {
            return;
        }
        m_editorLayer->m_player1->m_lastPosition = m_fields->previewKnob->getPosition();
        if (m_fields->previewKnob->getPositionX() < m_fields->previewRange.second) {
            m_fields->previewKnob->setPositionX(m_fields->previewKnob->getPositionX() + 120 * dt);
        }
        auto pos = m_fields->previewKnob->getPosition();
        m_editorLayer->m_player1->m_position = pos;
        m_editorLayer->m_player1->setPosition(pos);
        this->updateTriggerPreview(dt);
    }

    void updateTriggerPreview(float dt) {
        this->showUI(false);
        log::info("GJEffectManager::m_unkVector6c0: {}", m_editorLayer->m_effectManager->m_unkVector6c0.size());
        m_editorLayer->m_player1->setVisible(true);
        m_editorLayer->m_effectManager->updateTimers(dt, m_editorLayer->m_gameState.m_timeWarp);
        m_editorLayer->m_effectManager->prepareMoveActions(dt, false);
        m_editorLayer->processMoveActionsStep(dt, true);
        m_editorLayer->m_effectManager->postMoveActions();
    }
};
