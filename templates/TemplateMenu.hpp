#pragma once

#include <GDMake.h>
#include <GUI/CCControlExtension/CCScale9Sprite.h>

inline cocos2d::CCSprite* make_bspr(char c) {
    auto spr = cocos2d::CCSprite::create("GJ_button_01.png");

    auto label = cocos2d::CCLabelBMFont::create(std::string(1, c).c_str(), "bigFont.fnt");
    label->setPosition(spr->getContentSize().width / 2, spr->getContentSize().height / 2 + 3.0f);

    spr->addChild(label);

    return spr;
}

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

class TemplateMenu;

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
};

class TemplateButton : public gd::CCMenuItemSpriteExtra {
    protected:
        cocos2d::CCSprite* m_pSpr;
    
    public:
        static TemplateButton* create(TBlock const&, TemplateMenu*, bool);

        friend class TemplateMenu;
};

class TemplateMenu : public gd::EditButtonBar {
    public:
        enum TMode {
            kTModeNormal,
            kTModeEdit,
            kTModeEditVariants,
        };

    protected:
        const float offsetTop = 40.0f;

        Template* m_pTemplate;
        TMode m_nMode;
        cocos2d::CCSize m_obSize;
        cocos2d::CCMenu* m_pEditMenu;
        cocos2d::CCMenu* m_pVariantsMenu;
        cocos2d::CCMenu* m_pVariantsBtns;
        gd::CCMenuItemSpriteExtra* m_pVariantsCloseBtn;
        bool init(cocos2d::CCArray*, int, bool, int, int, cocos2d::CCPoint);

        void setupEditMenu();

        void onSaveEdit(cocos2d::CCObject*);
        void onCancelEdit(cocos2d::CCObject*);
        void onCloseVariantMenu(cocos2d::CCObject*);
        void onShowVariantMenu(cocos2d::CCObject*);
        void onSaveBlock(cocos2d::CCObject*);
        void onSaveBlockVariant(cocos2d::CCObject*);

        friend class TemplateButton;

    public:
        static TemplateMenu* create(cocos2d::CCArray*, cocos2d::CCPoint, int, bool, int, int);

        void showVariantMenu(std::vector<TBlock>);
        void switchMode(TMode);
};
