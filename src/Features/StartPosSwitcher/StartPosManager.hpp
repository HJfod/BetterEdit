#pragma once

#include <Geode/Geode.hpp>
#include <vector>
#include <algorithm>

using namespace geode::prelude;

class StartPosManager : public CCObject {
protected:
    std::vector<CCPoint> m_positions;
    CCPoint m_active;

    inline static StartPosManager* s_instance;
    void sort();
public:
    static StartPosManager* get();
    void setStartPositions(CCArray* objects);
    void setActive(CCPoint const& pos);
    void addStartPos(CCPoint const& pos);
    void moveStartPos(CCPoint const& before, CCPoint const& after);
    StartPosObject* getStartPosFromPoint(CCPoint const& pos);
    void next();
    void previous();
    void clear();
};