#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/GJGameLevel.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/GameObject.hpp>
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
    log::info("match called");
    if (std::holds_alternative<FromPoint>(g_startPos)) {
        if (std::get<FromPoint>(g_startPos) == pos) {
            log::info("matched!");
        }
        return std::get<FromPoint>(g_startPos) == pos;
    }
    return false;
}

void updateActiveStartPos(StartPosKind startPos) {
    g_startPos = startPos;
}

class $modify(PlayLayer) {
	void addObject(GameObject* obj) {
        if (std::holds_alternative<DefaultBehaviour>(g_startPos)) {
            PlayLayer::addObject(obj);
            return;
        }
        StartPosObject* old = nullptr;
        if (m_startPos) {
            old = m_startPos;
        }
		PlayLayer::addObject(obj);
        if (old) {
            m_startPos = old;
            m_playerStartPosition = old->getPosition();
        }
        if (obj->m_objectID == 31) {
            log::info("pos: {}, {}", obj->getPositionX(), obj->getPositionY());
            log::info("active startpos: {}, {}", m_startPos->getPositionX(), m_startPos->getPositionY());
            if (std::holds_alternative<FromObj>(g_startPos)) {
                auto startpos = std::get<FromObj>(g_startPos);
                log::info("global startpos: {}, {}", startpos->getPositionX(), startpos->getPositionY());
            }
            if (std::holds_alternative<FromPoint>(g_startPos)) {
                auto coords = std::get<FromPoint>(g_startPos);
                log::info("global startpos: {}, {}", coords.x, coords.y);
            }
            if (match(obj->getPosition())) {
                m_startPos = static_cast<StartPosObject*>(obj);
                m_playerStartPosition = obj->getPosition();
            }
            if (std::holds_alternative<FromLevelStart>(g_startPos)) {
                m_startPos = nullptr;
                m_playerStartPosition = CCPoint { 0, 105.f };
            }
        }
    }
};

class $modify(StartPosSwitchLayer, LevelEditorLayer) {
    bool init(GJGameLevel* level) {
        if (!LevelEditorLayer::init(level)) {
            return false;
        }

        g_startPos = DefaultBehaviour();
        return true;
    }

    void addSpecial(GameObject* obj) {
        log::info("add special");
        LevelEditorLayer::addSpecial(obj);
        if (obj->m_objectID == 31) {
            if (match(obj->getPosition())) {
                g_startPos = static_cast<StartPosObject*>(obj);
            }
        }
    }

    // void destructor() {
    //     PlaytestHerePopup::hide();
    //     LevelEditorLayer::~LevelEditorLayer();
    // }

    void handleAction(bool idk, CCArray* idk2) {
        LevelEditorLayer::handleAction(idk, idk2);
        PlaytestHerePopup::move();
    }

    void setupLevelStart(LevelSettingsObject* obj) {
        // selected start position
        if (std::holds_alternative<FromObj>(g_startPos)) {
            auto obj = std::get<FromObj>(g_startPos);
            this->setStartPosObject(obj);
            auto startPos = obj->getPosition();
            m_player1->setStartPos(startPos);
            m_player1->resetObject();
            m_player2->setStartPos(startPos);
            m_player2->resetObject();
            LevelEditorLayer::setupLevelStart(obj->m_levelSettings);
        }
        // from level start
        else if (std::holds_alternative<FromLevelStart>(g_startPos)) {
            this->setStartPosObject(nullptr);
            m_player1->setStartPos({ 0.f, 105.f });
            m_player1->resetObject();
            m_player2->setStartPos({ 0.f, 105.f });
            m_player2->resetObject();
            LevelEditorLayer::setupLevelStart(m_levelSettings);
        }
        // default behaviour
        else {
            LevelEditorLayer::setupLevelStart(obj);
        }
    }
};

$onEditorExit {
    PlaytestHerePopup::hide();
}


class $modify(EditorPauseLayer) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorPauseLayer::init(lel))
            return false;
        
        PlaytestHerePopup::hide();

        return true;
    }
};


class $modify(MyEditorUI, EditorUI) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;
        
        MoreTabs::get(this)->addEditTab(
            "start-pos-border.png"_spr,
            StartPosButtonBar::create(m_editorLayer, [](StartPosKind startPos) {
                g_startPos = startPos;
            })
        );

        return true;
    }

    void deleteObject(GameObject* obj, bool filter) {
        EditorUI::deleteObject(obj, filter);
        PlaytestHerePopup::hide();
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
        EditorUI::moveObject(obj, pos);
        PlaytestHerePopup::move();
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
                [](StartPosKind startPos) {
                    g_startPos = startPos;
                }
            );
        }
    }
};
