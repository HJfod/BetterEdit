#pragma once

#include <Geode/Geode.hpp>

#include "StartPosKind.hpp"

using namespace geode::prelude;

class StartPosButtonBar : public CCMenu {
protected:
    LevelEditorLayer* m_editor;
    std::function<void(StartPosKind)> m_callback;
    CCLabelBMFont* m_counterLabel = nullptr;

    bool init(LevelEditorLayer*, std::function<void(StartPosKind)>);
    void onInfo(CCObject*);
    void onGoToStart(CCObject*);
    void onGoToLast(CCObject*);
public:
    void setStartPosCounters(StartPosKind);
    static StartPosButtonBar* create(LevelEditorLayer* lel, std::function<void(StartPosKind)> callback) {
        auto ret = new StartPosButtonBar;
        if (ret && ret->init(lel, callback)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};