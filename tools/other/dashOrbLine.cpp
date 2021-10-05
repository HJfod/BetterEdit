#include "dashOrbLine.hpp"
#include "../VisibilityTab/loadVisibilityTab.hpp"

CCArray* g_pDashOrbs = nullptr;
CCArray* g_pDashOrbEnds = nullptr;

void mgetlinef(float x1, float y1, float x2, float y2, float &a, float &b, float &c) {
    // (x- p1X) / (p2X - p1X) = (y - p1Y) / (p2Y - p1Y) 
    a = y1 - y2; // Note: this was incorrectly "y2 - y1" in the original answer
    b = x2 - x1;
    c = x1 * y2 - x2 * y1;
}

float mdistf(float pct1X, float pct1Y, float pct2X, float pct2Y, float pct3X, float pct3Y) {
    float a, b, c;
    mgetlinef(pct2X, pct2Y, pct3X, pct3Y, a, b, c);
    return abs(a * pct1X + b * pct1Y + c) / sqrt(a * a + b * b);
}

void clearDashOrbLines() {
    g_pDashOrbs->removeAllObjects();
    g_pDashOrbEnds->removeAllObjects();
}

void loadDashOrbLines() {
    g_pDashOrbs = CCArray::create();
    g_pDashOrbs->retain();

    g_pDashOrbEnds = CCArray::create();
    g_pDashOrbEnds->retain();
}

void unloadDashOrbLines() {
    g_pDashOrbs->release();
    g_pDashOrbEnds->release();
}

void registerDashOrb(GameObject* obj) {
    if (
        obj->m_nObjectID == 1704 ||
        obj->m_nObjectID == 1751
    ) {
        g_pDashOrbs->addObject(obj);
    }

    if (
        obj->m_nObjectID == 1829
    ) {
        g_pDashOrbEnds->addObject(obj);
    }
}

void unregisterDashOrb(GameObject* obj) {
    g_pDashOrbs->removeObject(obj);
    g_pDashOrbEnds->removeObject(obj);
}

void drawDashOrbLines(DrawGridLayer* self) {
    if (!shouldShowDashLines())
        return;

    CCARRAY_FOREACH_B_TYPE(g_pDashOrbs, obj, GameObject) {
        switch (obj->m_nObjectID) {
            case 1704:  // green dash orb
                ccDrawColor4B(0, 255, 0, 255);
                break;

            case 1751:  // pink dash orb
                ccDrawColor4B(255, 90, 180, 255);
                break;  
        }
        auto opos =  obj->getPosition();
        auto len = 500.f;
        auto pos = opos + CCPoint {
            len * cosf((360.f - obj->getRotation()) * 3.1415926535897f / 180.f),
            len * sinf((360.f - obj->getRotation()) * 3.1415926535897f / 180.f)
        };
        CCARRAY_FOREACH_B_TYPE(g_pDashOrbEnds, end, GameObject) {
            auto epos = end->getPosition();
            auto nlen = ccpDistance(epos, opos) - 15.f;
            if (nlen < len) {
                auto ldis = mdistf(pos.x, pos.y, opos.x, opos.y, epos.x, epos.y);
                if (ldis < 60.f) {
                    len = nlen;
                    pos = opos + CCPoint {
                        len * cosf((360.f - obj->getRotation()) * 3.1415926535897f / 180.f),
                        len * sinf((360.f - obj->getRotation()) * 3.1415926535897f / 180.f)
                    };
                }
            }
        }
        ccDrawLine(obj->getPosition(), pos);
    }
}
