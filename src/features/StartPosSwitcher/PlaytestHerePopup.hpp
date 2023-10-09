#pragma once

#include <Geode/Geode.hpp>

#include "StartPosManager.hpp"

using namespace geode::prelude;

class PlaytestHerePopup : public CCNode {
protected:
    Ref<StartPosObject> m_startPos;
    LevelEditorLayer* m_editor;
    inline static PlaytestHerePopup* s_popup = nullptr;
    std::function<void()> m_callback;

    bool init(LevelEditorLayer*, StartPosObject*, std::function<void()>);

    static PlaytestHerePopup* create(
        LevelEditorLayer* lel, StartPosObject* startPos, std::function<void()> callback
    ) {
        auto ret = new PlaytestHerePopup;
        if (ret && ret->init(lel, startPos, callback)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    void onPlaytest(CCObject*);
    void onPlayInGame(CCObject*);
public:
    void select();

    static void move() {
        if (s_popup) {
            s_popup->setPosition(
                s_popup->m_startPos->getPositionX(),
                s_popup->m_startPos->getPositionY() + 35.f
            );
        }
    }

    static void show(LevelEditorLayer* lel, StartPosObject* startPos, std::function<void()> callback) {
        PlaytestHerePopup::hide();
        PlaytestHerePopup::create(lel, startPos, callback);
        PlaytestHerePopup::move();
        lel->m_objectLayer->addChild(s_popup);
    }

    static void hide() {
        if (s_popup) {
            s_popup->removeFromParent();
            s_popup = nullptr;
        }
    }
};