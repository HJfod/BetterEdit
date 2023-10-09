#pragma once

#include <Geode/Geode.hpp>
#include <vector>
#include <algorithm>

#include "StartPosManager.hpp"

using namespace geode::prelude;

class StartPosButtonBar : public CCMenu {
protected:
    LevelEditorLayer* m_editor;
    CCLabelBMFont* m_counterLabel = nullptr;

    int m_totalStartPositions = 0;
    int m_activeIndex = 0;

    bool init(LevelEditorLayer*);
public:
    void setStartPosCounters();
    void onInfo(CCObject*);
    void onGoToStart(CCObject*);
    void onGoToLast(CCObject*);
    void onNext(CCObject*);
    void onPrevious(CCObject*);
    static StartPosButtonBar* create(LevelEditorLayer* lel) {
        auto ret = new StartPosButtonBar;
        if (ret && ret->init(lel)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};