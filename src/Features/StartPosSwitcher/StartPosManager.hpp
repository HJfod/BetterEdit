#pragma once

#include <Geode/Geode.hpp>
#include <vector>
#include <algorithm>

using namespace geode::prelude;

class StartPosManager : public CCObject {
protected:
    std::vector<CCPoint> m_positions = {};
    CCPoint m_active = CCPointZero;
    bool m_initialized = false;

    inline static StartPosManager* s_instance;
    void sort();
public:
    static StartPosManager* get();
    void setStartPositions(CCArray* objects);
    void setActive(CCPoint const& pos);
    void addStartPos(CCPoint const& pos);
    void replaceStartPos(CCPoint const& before, CCPoint const& after);
    bool isDefault();
    bool isLevelStart();
    StartPosObject* getStartPosFromPoint(CCPoint const& pos);
    CCPoint getActive();
    std::vector<CCPoint> getPositions();
    void next();
    void previous();
    void clear();
    void setDefault();
    void setFirst();
};