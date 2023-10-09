#pragma once

#include <Geode/Geode.hpp>
#include <vector>
#include <algorithm>

#include "StartPosKind.hpp"

using namespace geode::prelude;

enum StartPositionSearchType {
    FindHigher,
    FindSmaller
};

class StartPosButtonBar : public CCMenu {
protected:
    LevelEditorLayer* m_editor;
    std::function<void(StartPosKind)> m_callback;
    CCLabelBMFont* m_counterLabel = nullptr;
    StartPosKind m_active = DefaultBehaviour();

    int m_totalStartPositions = 0;
    int m_activeIndex = 0;

    bool init(LevelEditorLayer*, std::function<void(StartPosKind)>);
    StartPosKind search(StartPosKind current, StartPositionSearchType searchType);
public:
    void setStartPosCounters(StartPosKind);
    void onInfo(CCObject*);
    void onGoToStart(CCObject*);
    void onGoToLast(CCObject*);
    void onNext(CCObject*);
    void onPrevious(CCObject*);
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