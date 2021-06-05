#pragma once

#include <GDMake.h>

class TemplateMenu;
class TemplateManager;

struct TBlock {
    enum TEdge : uint8_t {
        eNone   = 0,
        eLeft   = 1 << 0,
        eRight  = 1 << 1,
        eTop    = 1 << 2,
        eBottom = 1 << 3,
    };
    uint8_t edges = eNone;

    enum TCorner : uint8_t {
        cNone   = 0,
        cNW     = 1 << 4,
        cNE     = 1 << 5,
        cSW     = 1 << 6,
        cSE     = 1 << 7,
    };
    uint8_t corners = cNone;

    inline uint8_t getID() const {
        return edges | corners;
    };

    inline uint8_t getEdgeCount() const {
        uint8_t res;
        uint8_t c = edges;
        
        for (auto ix = 0u; ix < 4; ix++) {
            res += c & 1;
            c >>= 1;
        }

        return res;
    }
    
    inline uint8_t getCornerCount() const {
        uint8_t res;
        uint8_t c = corners;
        
        for (auto ix = 0u; ix < 4; ix++) {
            res += c & 1;
            c >>= 1;
        }

        return res;
    }
};

class Template : public cocos2d::CCObject {
    protected:
        std::map<TBlock, std::string> m_mObjects;

        inline bool init() {
            return true;
        }
    
    public:
        inline static Template* create() {
            auto ret = new Template();

            if (ret && ret->init()) {
                ret->autorelease();
                return ret;
            }

            CC_SAFE_DELETE(ret);
            return nullptr;
        }

        inline std::map<TBlock, std::string> getBlocks() const { return m_mObjects; }

        friend class TemplateMenu;
        friend class TemplateManager;
};

class TemplateManager : public gd::GManager {
    std::vector<Template*> m_vTemplates;

    virtual bool init() override;

    virtual void encodeDataTo(DS_Dictionary* data);
    virtual void dataLoaded(DS_Dictionary* data);
    virtual void firstLoad();

    public:
        static TemplateManager* sharedState();
};
