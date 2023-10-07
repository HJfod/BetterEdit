#include <Geode/utils/cocos.hpp>
#include <Other/BEShared.hpp>
#include <Geode/modify/DrawGridLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/binding/GameObject.hpp>

using namespace geode::prelude;

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

CCArray* g_dashOrbs = nullptr;
CCArray* g_dashOrbEnds = nullptr;

class $modify(DashOrbLineLayer, DrawGridLayer) {
    bool init(CCNode* grid, LevelEditorLayer* editor) {
        if (!DrawGridLayer::init(grid, editor))
            return false;

        if (!g_dashOrbs) {
            g_dashOrbs = CCArray::create();
            g_dashOrbs->retain();
        }

        if (!g_dashOrbEnds) {
            g_dashOrbEnds = CCArray::create();
            g_dashOrbEnds->retain();
        }

        return true;
    }

    static DashOrbLineLayer* get() {
        return static_cast<DashOrbLineLayer*>(DrawGridLayer::get());
    }

    void draw() {
        DrawGridLayer::draw();

        if (!Mod::get()->getSettingValue<bool>("dash-lines"))
            return;

        ccColor4B lineColor = {0, 0, 0, 0};

        for(auto& obj : CCArrayExt<GameObject*>(g_dashOrbs)) {
            switch (obj->m_objectID) {
                case 1704: // green dash orb
                    lineColor = {0, 255, 0, 255};
                    break;

                case 1751: // pink dash orb
                    lineColor = {255, 90, 180, 255};
                    break;
            }

            auto objPos = obj->getPosition();
            float len = 500.f;
            auto pos = objPos + CCPoint {
                len * cosf((360.f - obj->getRotation()) * 3.1415926535897f / 180.f),
                len * sinf((360.f - obj->getRotation()) * 3.1415926535897f / 180.f)
            };

            for(auto& end : CCArrayExt<GameObject*>(g_dashOrbEnds)) {
                auto endPos = end->getPosition();
                auto nlen = ccpDistance(endPos, objPos) - 15.f;

                if (nlen < len) {
                    auto ldis = mdistf(pos.x, pos.y, objPos.x, objPos.y, endPos.x, endPos.y);
                    if (ldis < 60.f) {
                        len = nlen;
                        pos = objPos + CCPoint {
                            len * cosf((360.f - obj->getRotation()) * 3.1415926535897f / 180.f),
                            len * sinf((360.f - obj->getRotation()) * 3.1415926535897f / 180.f)
                        };
                    }
                }
            }

            // draw
            ccDrawColor4B(lineColor);
            ccDrawLine(obj->getPosition(), pos);
        }
    }

    void registerDashOrb(GameObject* obj) {
        if (obj->m_objectID == 1704 || obj->m_objectID == 1751)
            g_dashOrbs->addObject(obj);

        if (obj->m_objectID == 1829)
            g_dashOrbEnds->addObject(obj);
    }

    void unregisterDashOrb(GameObject* obj) {
        g_dashOrbs->removeObject(obj);
        g_dashOrbEnds->removeObject(obj);
    }
};

class $modify(LevelEditorLayer) {
    void addSpecial(GameObject* obj) {
        LevelEditorLayer::addSpecial(obj);

        DashOrbLineLayer::get()->registerDashOrb(obj);
    }

    void removeObject(GameObject* obj, bool p1) {
        LevelEditorLayer::removeObject(obj, p1);

        DashOrbLineLayer::get()->unregisterDashOrb(obj);
    }
};

BE_EDITOREXIT() {
    auto self = DashOrbLineLayer::get();

    #define CLEAR_ARRAY(array)                          \
        for (auto& obj : CCArrayExt<CCObject*>(array)) {\
            array->removeObject(obj, false);            \
        }

    if (g_dashOrbs) {
        CLEAR_ARRAY(g_dashOrbs);
        g_dashOrbs->release();
        g_dashOrbs = nullptr;
    }

    if (g_dashOrbEnds) {
        CLEAR_ARRAY(g_dashOrbEnds);
        g_dashOrbEnds->release();
        g_dashOrbEnds = nullptr;
    }
}
