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
#include "StartPosKind.hpp"
#include "StartPosButtonBar.hpp"

using namespace geode::prelude;
using namespace editor_api;

StartPosKind g_startPos = DefaultBehaviour();

bool match(CCPoint const& pos) {
    if (std::holds_alternative<FromPoint>(g_startPos)) {
        return std::get<FromPoint>(g_startPos) == pos;
    }
    return false;
}

/**
 * Fetches the startpos that matches the given position. If objects is passed, it searches the objects, if not it tries to look in LevelEditorLayer and PlayLayer. 
 * If none is found, returns nullptr.
*/
StartPosObject* getStartPosByPosition(CCPoint const& pos, CCArray* objects) {
    if (!objects) {
        auto levelEdit = LevelEditorLayer::get();
        if (levelEdit) {
            objects = levelEdit->m_objects;
        }

        auto playLayer = PlayLayer::get();
        if (!objects && playLayer) {
            objects = playLayer->m_objects;
        }

        if (!objects) {
            return nullptr;
        }
    }

    for (auto object : CCArrayExt<GameObject*>(objects)) {
        auto editorUI = EditorUI::get();
        if (object->m_objectID == 31) {
            if (object->getPosition() == pos) {
                return static_cast<StartPosObject*>(object);
            }
            // attention please... THE HACKIEST FIX!!
            if (editorUI && object->getPosition() == editorUI->getGridSnappedPos(pos)) {
                g_startPos = editorUI->getGridSnappedPos(pos);
                return static_cast<StartPosObject*>(object);
            }
        }
    }

    return nullptr;
}

class $modify(PlayLayer) {
    StartPosObject* activeStartPos = nullptr;
    bool fromLevelStart = false;
	void addObject(GameObject* obj) {
        PlayLayer::addObject(obj);
        if (std::holds_alternative<DefaultBehaviour>(g_startPos)) {
            PlayLayer::addObject(obj);
            return;
        }
		PlayLayer::addObject(obj);
        if (obj->m_objectID == 31) {
            if (match(obj->getPosition())) {
                m_fields->activeStartPos = static_cast<StartPosObject*>(obj);
            }
            if (std::holds_alternative<FromLevelStart>(g_startPos)) {
                m_fields->fromLevelStart = true;
            }

            if (m_fields->fromLevelStart) {
                m_startPos = nullptr;
                m_playerStartPosition = CCPointZero;
                return;
            }

            if (m_fields->activeStartPos) {
                m_startPos = m_fields->activeStartPos;
                m_playerStartPosition = m_fields->activeStartPos->getPosition();
            }
        }
    }
};

class $modify(StartPosSwitchLayer, LevelEditorLayer) {
    void addSpecial(GameObject* obj) {
        LevelEditorLayer::addSpecial(obj);
        if (!m_editorInitialising) {
            auto bar = static_cast<StartPosButtonBar*>(m_editorUI->getChildByID("start-pos-button-bar"_spr));
            if (bar) {
                bar->setStartPosCounters(g_startPos);
            }
        }
    }

    void handleAction(bool idk, CCArray* idk2) {
        LevelEditorLayer::handleAction(idk, idk2);
        PlaytestHerePopup::move();
    }

    void setupLevelStart(LevelSettingsObject* obj) {
        if (std::holds_alternative<FromPoint>(g_startPos)) {
            auto pos = std::get<FromPoint>(g_startPos);
            auto startPos = getStartPosByPosition(pos, m_objects);
            if (!startPos) {
                Notification::create("Couldn't setup startpos switcher.", CCSprite::createWithSpriteFrameName("edit_delBtnSmall_001.png"))->show();
                g_startPos = DefaultBehaviour();
                auto bar = static_cast<StartPosButtonBar*>(m_editorUI->getChildByID("start-pos-button-bar"_spr));
                if (bar) {
                    bar->setStartPosCounters(g_startPos);
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
            return;
        }
        if (std::holds_alternative<FromLevelStart>(g_startPos)) {
            this->setStartPosObject(nullptr);
            m_player1->setStartPos(CCPointZero);
            m_player1->resetObject();
            m_player2->setStartPos(CCPointZero);
            m_player2->resetObject();
            LevelEditorLayer::setupLevelStart(m_levelSettings);
            return;
        }
        // default behaviour
        LevelEditorLayer::setupLevelStart(obj);
    }
};

$onEditorExit {
    PlaytestHerePopup::hide();
}

// In case $onEditorExit runs on save and play
class $modify(GameManager) {
    void returnToLastScene(GJGameLevel* level) {
        GameManager::returnToLastScene(level);
        g_startPos = DefaultBehaviour();
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
        if (std::holds_alternative<FromPoint>(g_startPos)) {
            auto startpos = getStartPosByPosition(std::get<FromPoint>(g_startPos), m_editorLayer->m_objects);
        }
        EditorPauseLayer::onSaveAndPlay(sender);
    }

    void onExitEditor(CCObject* sender) {
        EditorPauseLayer::onExitEditor(sender);
        g_startPos = DefaultBehaviour();
    }

    void onSaveAndExit(CCObject* sender) {
        EditorPauseLayer::onSaveAndExit(sender);
        g_startPos = DefaultBehaviour();
    }
};

class $modify(MyEditorUI, EditorUI) {
    StartPosButtonBar* buttonBar = nullptr;
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;

        auto buttonBar = StartPosButtonBar::create(m_editorLayer, [](StartPosKind startPos) {
                g_startPos = startPos;
            });
        buttonBar->setID("start-pos-button-bar"_spr);
        m_fields->buttonBar = buttonBar;
        
        MoreTabs::get(this)->addEditTab(
            "start-pos-border.png"_spr,
            buttonBar
        );

        if (std::holds_alternative<DefaultBehaviour>(g_startPos)) {
            return true;
        }
        buttonBar->setStartPosCounters(g_startPos);
        return true;
    }

    void onDeleteStartPos(CCObject* sender) {
        EditorUI::onDeleteStartPos(sender);
        g_startPos = DefaultBehaviour();
        m_fields->buttonBar->setStartPosCounters(g_startPos);
    }

    void deleteObject(GameObject* obj, bool filter) {
        EditorUI::deleteObject(obj, filter);
        PlaytestHerePopup::hide();
        if (obj->m_objectID == 31 && std::holds_alternative<FromPoint>(g_startPos)) {
            auto pos = std::get<FromPoint>(g_startPos);
            if (obj->getPosition() == pos) {
                g_startPos = DefaultBehaviour();
            }
        }
        m_fields->buttonBar->setStartPosCounters(g_startPos);
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
        bool movedActiveStartPos = false;
        if (!m_editorLayer->m_editorInitialising) {
            if (obj->m_objectID == 31 && std::holds_alternative<FromPoint>(g_startPos)) {
                auto position = std::get<FromPoint>(g_startPos);
                if (position == obj->getPosition()) {
                    movedActiveStartPos = true;
                }
            }
        }
        EditorUI::moveObject(obj, pos);
        PlaytestHerePopup::move();
        if (movedActiveStartPos) {
            g_startPos = obj->getPosition();
            m_fields->buttonBar->setStartPosCounters(g_startPos);
        }
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
                [this](StartPosKind startPos) {
                    g_startPos = startPos;
                    auto buttonBar = static_cast<StartPosButtonBar*>(LevelEditorLayer::get()->m_editorUI->getChildByID("start-pos-button-bar"_spr));
                    if (buttonBar) {
                        buttonBar->setStartPosCounters(startPos);
                    }
                }
            );
        }
    }
};
