#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/GJGameLevel.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/GameManager.hpp>
#include <MoreTabs.hpp>
#include <Utils.hpp>
#include <Other/BEShared.hpp>
#undef min
#undef max

#include "PlaytestHerePopup.hpp"
#include "StartPosButtonBar.hpp"
#include "StartPosManager.hpp"

using namespace geode::prelude;
using namespace editor_api;

struct RecalculateStartPosData {
    CCPoint original;
    CCPoint newPoint;
};

class $modify(PlayLayer) {
    StartPosObject* activeStartPos = nullptr;
    bool fromLevelStart = false;
	void addObject(GameObject* obj) {
        PlayLayer::addObject(obj);
        if (obj->m_objectID != 31) {
            return;
        }
        if (StartPosManager::get()->isDefault()) {
            return;
        }
        if (StartPosManager::get()->isLevelStart()) {
            m_startPos = nullptr;
            m_playerStartPosition = CCPointZero;
            return;
        }

        auto active = StartPosManager::get()->getActive();
        if (obj->getPosition() == StartPosManager::get()->getActive() && !m_fields->activeStartPos) {
            m_fields->activeStartPos = static_cast<StartPosObject*>(obj);
        }

        if (m_fields->activeStartPos) {
            m_startPos = m_fields->activeStartPos;
            m_playerStartPosition = m_fields->activeStartPos->getPosition();
        }
    }
};

class $modify(StartPosSwitchLayer, LevelEditorLayer) {
    bool init(GJGameLevel* level) {
        if (!LevelEditorLayer::init(level)) {
            return false;
        }
        StartPosManager::get()->setStartPositions(m_objects);
        auto bar = static_cast<StartPosButtonBar*>(m_editorUI->getChildByID("start-pos-button-bar"_spr));
        if (bar) {
            bar->setStartPosCounters();
        }

        return true;
    }
    void addSpecial(GameObject* obj) {
        LevelEditorLayer::addSpecial(obj);
        if (!m_editorInitialising && obj->m_objectID == 31) {
            StartPosManager::get()->addStartPos(obj->getPosition());
            auto bar = static_cast<StartPosButtonBar*>(m_editorUI->getChildByID("start-pos-button-bar"_spr));
            if (bar) {
                bar->setStartPosCounters();
            }
        }
    }

    void handleAction(bool idk, CCArray* idk2) {
        LevelEditorLayer::handleAction(idk, idk2);
        PlaytestHerePopup::move();
    }

    void setupLevelStart(LevelSettingsObject* obj) {
        StartPosManager::get()->setStartPositions(m_objects);
        if (StartPosManager::get()->isLevelStart()) {
            this->setStartPosObject(nullptr);
            m_player1->setStartPos(CCPointZero);
            m_player1->resetObject();
            m_player2->setStartPos(CCPointZero);
            m_player2->resetObject();
            LevelEditorLayer::setupLevelStart(m_levelSettings);
            return;
        }
        auto active = StartPosManager::get()->getActive();
        auto startPos = StartPosManager::get()->getStartPosFromPoint(active);
        if (!startPos) {
            Notification::create("Couldn't setup Start Position switcher.", CCSprite::createWithSpriteFrameName("edit_delBtnSmall_001.png"))->show();
            StartPosManager::get()->setDefault();
            auto bar = static_cast<StartPosButtonBar*>(m_editorUI->getChildByID("start-pos-button-bar"_spr));
            if (bar) {
                bar->setStartPosCounters();
            }
            LevelEditorLayer::setupLevelStart(obj);
            return;
        }

        this->setStartPosObject(startPos);
        m_player1->setStartPos(startPos->getPosition());
        m_player1->resetObject();
        m_player2->setStartPos(startPos->getPosition());
        m_player2->resetObject();
        LevelEditorLayer::setupLevelStart(startPos->m_levelSettings);
    }
};

$onEditorExit {
    PlaytestHerePopup::hide();
}

// In case $onEditorExit runs on save and play
class $modify(GameManager) {
    void returnToLastScene(GJGameLevel* level) {
        GameManager::returnToLastScene(level);
        StartPosManager::get()->clear();
    }
};

class $modify(EditorPauseLayer) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorPauseLayer::init(lel))
            return false;
        
        PlaytestHerePopup::hide();

        return true;
    }

    void onSaveAndPlay(CCObject* sender) {
        StartPosManager::get()->setStartPositions(m_editorLayer->m_objects);
        EditorPauseLayer::onSaveAndPlay(sender);
    }

    void onExitEditor(CCObject* sender) {
        EditorPauseLayer::onExitEditor(sender);
        StartPosManager::get()->clear();
    }

    void onSaveAndExit(CCObject* sender) {
        EditorPauseLayer::onSaveAndExit(sender);
        StartPosManager::get()->clear();
    }
};

class $modify(MyEditorUI, EditorUI) {
    StartPosButtonBar* buttonBar = nullptr;

    bool holding = false;
    std::unordered_map<GameObject*, CCPoint> startPosOriginals;
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel)) {
            return false;
        }
        auto buttonBar = StartPosButtonBar::create(m_editorLayer);        
        buttonBar->setID("start-pos-button-bar"_spr);
        m_fields->buttonBar = buttonBar;
        
        MoreTabs::get(this)->addEditTab(
            "start-pos-border.png"_spr,
            buttonBar
        );
        return true;
    }

    void onDeleteStartPos(CCObject* sender) {
        EditorUI::onDeleteStartPos(sender);
        StartPosManager::get()->clear();
        StartPosManager::get()->setStartPositions(nullptr);
        m_fields->buttonBar->setStartPosCounters();
    }

    void deleteObject(GameObject* obj, bool filter) {
        EditorUI::deleteObject(obj, filter);
        PlaytestHerePopup::hide();
        if (obj->m_objectID == 31) {
            auto manager = StartPosManager::get();
            if (obj->getPosition() == manager->getActive()) {
                manager->clear();
            }
            manager->setStartPositions(m_editorLayer->m_objects);
            m_fields->buttonBar->setStartPosCounters();
        }
    }

    void selectObjects(CCArray* objects, bool ignoreFilters) {
        EditorUI::selectObjects(objects, ignoreFilters);
        PlaytestHerePopup::hide();
    }

    void selectObject(GameObject* obj, bool filter) {
        EditorUI::selectObject(obj, filter);
        MoreTabs::get(this)->switchEditTab(1);
    }

    void moveObject(GameObject* obj, CCPoint pos) {
        if (m_editorLayer->m_editorInitialising) {
            EditorUI::moveObject(obj, pos);
            PlaytestHerePopup::move();
            return;
        }
        bool movedStartPos = false;
        CCPoint original;
        if (obj->m_objectID == 31) {
            movedStartPos = true;
            original = obj->getPosition();
        }
        EditorUI::moveObject(obj, pos);
        PlaytestHerePopup::move();
        if (movedStartPos) {
            // if holding, add to map and recalculate once when touch ends
            if (m_fields->holding && !m_fields->startPosOriginals.contains(obj)) {
                m_fields->startPosOriginals[obj] = original;
                return;
            }
            if (m_fields->holding) {
                return;
            }
            StartPosManager::get()->replaceStartPos(original, obj->getPosition());
            m_fields->buttonBar->setStartPosCounters();
        }
    }

    // doing moving with freemove a lil better
    bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
        auto ret = EditorUI::ccTouchBegan(touch, event);
        m_fields->holding = true;

        // bad fix for bad bug :)
        if (m_freeMoveEnabled) {
            MoreTabs::get(this, false)->switchEditTab(1);
        }
        return ret;
    }

    // if we are free moving any startpos, update the manager and clear the map
    void ccTouchEnded(CCTouch* touch, CCEvent* event) {
        EditorUI::ccTouchEnded(touch, event);
        for (const auto& kv : m_fields->startPosOriginals) {
            StartPosManager::get()->replaceStartPos(kv.second, kv.first->getPosition());
        }
        m_fields->buttonBar->setStartPosCounters();
        m_fields->startPosOriginals.clear();
        m_fields->holding = false;
    }

    void moveObjectCall(EditCommand cmd) {
        EditorUI::moveObjectCall(cmd);
        PlaytestHerePopup::move();
    }

    void deselectAll() {
        EditorUI::deselectAll();
        PlaytestHerePopup::hide();
    }
};

class $modify(GameObject) {
    void selectObject(ccColor3B color) {
        GameObject::selectObject(color);
        PlaytestHerePopup::hide();
        if (m_objectID == 31) {
            PlaytestHerePopup::show(
                LevelEditorLayer::get(),
                static_cast<StartPosObject*>(as<GameObject*>(this)),
                [this]() {
                    auto buttonBar = static_cast<StartPosButtonBar*>(LevelEditorLayer::get()->m_editorUI->getChildByID("start-pos-button-bar"_spr));
                    if (buttonBar) {
                        buttonBar->setStartPosCounters();
                    }
                }
            );
        }
    }
};
