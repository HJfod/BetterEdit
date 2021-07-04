#include "ColorTriggerPopup.hpp"

using namespace gd;
using namespace gdmake;
using namespace gdmake::extra;
using namespace cocos2d;

void ColorTriggerPopup::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->m_pLayer->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(60.0f, "0", "0123456789"_s))
            .move(winSize.width / 2 - 60.0f, winSize.height / 2)
            .save(&m_pRangeStartInput)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(60.0f, "0", "0123456789"_s))
            .move(winSize.width / 2 + 60.0f, winSize.height / 2)
            .save(&m_pRangeEndInput)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<CCLabelBMFont*>()
            .fromText("-", "bigFont.fnt")
            .move(winSize / 2)
            .scale(.7f)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<CCLabelBMFont*>()
            .fromText("Start ID:", "goldFont.fnt")
            .move(winSize.width / 2 - 60.0f, winSize.height / 2 + 30.0f)
            .scale(.6f)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<CCLabelBMFont*>()
            .fromText("End ID:", "goldFont.fnt")
            .move(winSize.width / 2 + 60.0f, winSize.height / 2 + 30.0f)
            .scale(.6f)
            .done()
    );

    this->m_pButtonMenu->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<ButtonSprite*>()
                    .fromNode(ButtonSprite::create(
                        "Create", 0, 0, "goldFont.fnt", "GJ_button_01.png", 0, .8f
                    ))
                    .scale(.8f)
                    .done(),
                this,
                (SEL_MenuHandler)&ColorTriggerPopup::onCreate
            ))
            .move(0.0f, - this->m_pLrSize.height / 2 + 25.0f)
            .done()
    );
}

void ColorTriggerPopup::onCreate(CCObject* pSender) {
    // logic code here...

    int rangeStart = 0;
    int rangeEnd = 1012;

    if (this->m_pRangeStartInput &&
        this->m_pRangeStartInput->getString() &&
        strlen(this->m_pRangeStartInput->getString()))
        rangeStart = std::atoi(this->m_pRangeStartInput->getString());

    if (this->m_pRangeEndInput &&
        this->m_pRangeEndInput->getString() &&
        strlen(this->m_pRangeEndInput->getString()))
        rangeEnd = std::atoi(this->m_pRangeEndInput->getString());
    
    std::cout << rangeStart << " - " << rangeEnd << "\n";

    if (this->m_pPauseLayer)
        this->m_pPauseLayer->onResume(pSender);

    this->onClose(pSender);
}

ColorTriggerPopup* ColorTriggerPopup::create(EditorPauseLayer* pl) {
    auto pRet = new ColorTriggerPopup();

    if (pRet && pRet->init(250.0f, 170.0f, "GJ_square01.png", "Create Color Triggers")) {
        pRet->m_pPauseLayer = pl;
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}

