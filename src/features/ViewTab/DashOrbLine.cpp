#include <Geode/utils/cocos.hpp>
#include <Geode/modify/DrawGridLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/binding/GameObject.hpp>
#include <numbers>

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

class $modify(DashOrbLineLayer, DrawGridLayer) {
    struct Fields {
        std::unordered_set<Ref<GameObject>> dashOrbs;
        std::unordered_set<Ref<GameObject>> dashOrbEnds;
    };

    $override
    void draw() {
        DrawGridLayer::draw();

        if (!Mod::get()->template getSavedValue<bool>("show-dash-lines"))
            return;

        for (auto obj : m_fields->dashOrbs) {
            auto lineColor = ccc4(255, 255, 255, 55);
            switch (obj->m_objectID) {
                // green dash orb 
                case 1704: lineColor = { 0, 255, 0, 255 }; break;
                // pink dash orb
                case 1751: lineColor = { 255, 90, 180, 255 }; break;
            }

            auto objPos = obj->getPosition();
            float len = 500.f;
            auto pos = objPos + CCPoint {
                len * cosf((360.f - obj->getRotation()) * std::numbers::pi_v<float> / 180.f),
                len * sinf((360.f - obj->getRotation()) * std::numbers::pi_v<float> / 180.f)
            };

            for (auto end : m_fields->dashOrbEnds) {
                auto endPos = end->getPosition();
                auto nlen = ccpDistance(endPos, objPos) - 15.f;

                if (nlen < len) {
                    auto ldis = mdistf(pos.x, pos.y, objPos.x, objPos.y, endPos.x, endPos.y);
                    if (ldis < 60.f) {
                        len = nlen;
                        pos = objPos + CCPoint {
                            len * cosf((360.f - obj->getRotation()) * std::numbers::pi_v<float> / 180.f),
                            len * sinf((360.f - obj->getRotation()) * std::numbers::pi_v<float> / 180.f)
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
        if (obj->m_objectID == 1704 || obj->m_objectID == 1751) {
            m_fields->dashOrbs.insert(obj);
        }
        if (obj->m_objectID == 1829) {
            m_fields->dashOrbEnds.insert(obj);
        }
    }
    void unregisterDashOrb(GameObject* obj) {
        m_fields->dashOrbs.erase(obj);
        m_fields->dashOrbEnds.erase(obj);
    }
};

class $modify(LevelEditorLayer) {
    $override
    void addSpecial(GameObject* obj) {
        LevelEditorLayer::addSpecial(obj);
        static_cast<DashOrbLineLayer*>(m_drawGridLayer)->registerDashOrb(obj);
    }
    // todo: this hook may have an impact on load performance - if that ever becomes a reported issue, check this!
    $override
    void removeSpecial(GameObject* obj) {
        LevelEditorLayer::removeSpecial(obj);
        static_cast<DashOrbLineLayer*>(m_drawGridLayer)->unregisterDashOrb(obj);
    }
};
