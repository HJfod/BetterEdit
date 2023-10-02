#include <Geode/utils/cocos.hpp>
#include <other/BEShared.hpp>

#include <Geode/modify/DrawGridLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

using namespace geode::prelude;

CCArray* g_guides2 = nullptr;
CCDrawNode* g_portalDrawNode = nullptr;

#ifdef GEODE_IS_WINDOWS

class $modify(PortalLinesLayer, DrawGridLayer) {
    bool init(CCNode* grid, LevelEditorLayer* editor) {
        if(!DrawGridLayer::init(grid, editor))
            return false;

        if(g_guides2 == nullptr) {
            g_guides2 = CCArray::create();
            g_guides2->retain();
        }

        if(g_portalDrawNode == nullptr) {
            g_portalDrawNode = CCDrawNode::create();
            g_portalDrawNode->retain();

            this->addChild(g_portalDrawNode, 1000);
        }
        
        return true;
    }

    static PortalLinesLayer* get() {
        return static_cast<PortalLinesLayer*>(DrawGridLayer::get());
    }

    void draw() {
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        bool portalLines = Mod::get()->getSettingValue<bool>("portal-lines");

        if(g_portalDrawNode != nullptr)
            g_portalDrawNode->clear();
        
        DrawGridLayer::draw();

        if(portalLines && g_guides2 != nullptr) {
            for(auto& portal : CCArrayExt<GameObject*>(g_guides2)) {
                auto portalPoints = getPortalMinMax(portal); // X is min, Y is max
                auto gridPosX = m_grid->convertToNodeSpace(CCPointZero).x;

                float startX = gridPosX > 0 ? gridPosX : 0;
                float endX = gridPosX + winSize.width / m_grid->getScale();
                endX = endX < 240000 ? endX : 240000;

                CCPoint startPoint;
                CCPoint endPoint;

                // line color
                ccColor4F lineColor = ccc4FFromccc4B({0, 255, 255, 255});

                if(Mod::get()->getSettingValue<bool>("color-gm-borders")) {
                    switch(portal->m_objectID) {
                        case 12:  lineColor = ccc4FFromccc4B({88,  255, 100, 255}); break; // cube
                        case 13:  lineColor = ccc4FFromccc4B({255, 150, 255, 255}); break; // ship
                        case 47:  lineColor = ccc4FFromccc4B({255, 34,  0,   255}); break; // ball
                        case 111: lineColor = ccc4FFromccc4B({255, 214, 85,  255}); break; // ufo
                        case 660: lineColor = ccc4FFromccc4B({31,  221, 255, 255}); break; // wave
                        case 745: lineColor = ccc4FFromccc4B({222, 221, 225, 255}); break; // robot
                        case 1331:lineColor = ccc4FFromccc4B({116, 21,  255, 255}); break; // spider
                    }
                }

                // bottom line
                if(portalPoints.x >= -90 && portalPoints.x <= 2490) {
                    startPoint = CCPoint(startX, portalPoints.x);
                    endPoint = CCPoint(endX, portalPoints.x);

                    if(g_portalDrawNode != nullptr)
                        g_portalDrawNode->drawSegment(startPoint, endPoint, 1, lineColor);
                }

                // top line
                if(portalPoints.y >= -90 && portalPoints.y <= 2490) {
                    startPoint = CCPoint(startX, portalPoints.y);
                    endPoint = CCPoint(endX, portalPoints.y);

                    if(g_portalDrawNode != nullptr)
                        g_portalDrawNode->drawSegment(startPoint, endPoint, 1, lineColor);
                }
            }
        }
    }

    // getPortalMinMax recreation
    CCPoint getPortalMinMax(GameObject* portal) {
        float min, portalHeight;

        if(portal->getType() == GameObjectType::BallPortal) {
            portalHeight = 260.f;
        }
        else {
            portalHeight = 320.f;
            if(portal->getType() == GameObjectType::SpiderPortal)
                portalHeight = 290.f;
        }

        min = floorf((portal->getPositionY() - portalHeight / 2 + 10) / 30) * 30;
        if(min <= 90)
            min = 90.f;

        return { min, min + portalHeight - 20 };
    }  

    void updateGuides() {
        // reset
        for(auto& obj : CCArrayExt<CCObject*>(m_guides)) {
            m_guides->removeObject(obj, false);
        }
        for(auto& obj : CCArrayExt<CCObject*>(g_guides2)) {
            g_guides2->removeObject(obj, false);
        }

        // add every portal to new array
        for(auto& obj : CCArrayExt<GameObject*>(m_editor->getAllObjects())) {
            if( obj->m_objectID == 12
            ||  obj->m_objectID == 13
            ||  obj->m_objectID == 47
            ||  obj->m_objectID == 111
            ||  obj->m_objectID == 660
            ||  obj->m_objectID == 745
            ||  obj->m_objectID == 1331
            ) {
                if(obj->m_showGamemodeBorders)
                    g_guides2->addObject(obj);
            }
        }
    }
};

class $modify(EditorUI) {
    void onToggleGuide(CCObject* pSender) {
        EditorUI::onToggleGuide(pSender);

        PortalLinesLayer::get()->updateGuides();
    }
};

class $modify(LevelEditorLayer) {
    void addSpecial(GameObject* obj) {
        LevelEditorLayer::addSpecial(obj);

        if(!m_editorInitialising)
            PortalLinesLayer::get()->updateGuides();
    }

    bool init(GJGameLevel* level) {
        if(!LevelEditorLayer::init(level))
            return false;

        PortalLinesLayer::get()->updateGuides();

        return true;
    }
};

BE_EDITOREXIT() {
    if(g_guides2 != nullptr) {
        for(auto& obj : CCArrayExt<GameObject*>(g_guides2))
            g_guides2->removeObject(obj, false);

        g_guides2->release();
        g_guides2 = nullptr;
    }

    if(g_portalDrawNode != nullptr) {
        g_portalDrawNode->removeFromParentAndCleanup(true);
        g_portalDrawNode = nullptr;
    }
}

#endif