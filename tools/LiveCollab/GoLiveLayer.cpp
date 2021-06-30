#include "GoLiveLayer.hpp"

using namespace gd;
using namespace gdmake;
using namespace gdmake::extra;
using namespace cocos2d;

void GoLiveLayer::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto pHostButtons = CCArray::create();
    auto pJoinButtons = CCArray::create();

    this->m_pLayer->addChild(
        CCNodeConstructor<CCLabelBMFont*>()
            .fromText("IP Address", "bigFont.fnt")
            .move(winSize.width / 2, winSize.height / 2 + 30.0f)
            .scale(.6f)
            .exec([&pJoinButtons](auto s) -> void { pJoinButtons->addObject(s); })
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(150.0f, "0.0.0.0:0000"))
            .move(winSize / 2)
            .exec([&pJoinButtons](auto s) -> void { pJoinButtons->addObject(s); })
            .done()
    );

    this->m_pLayer->addChild(
        CCNodeConstructor<CCLabelBMFont*>()
            .fromText("Host Room", "goldFont.fnt")
            .move(winSize / 2)
            .scale(.7f)
            .exec([&pHostButtons](auto s) -> void { pHostButtons->addObject(s); })
            .done()
    );

    this->m_pButtonMenu->addChild(
        CCNodeConstructor<ButtonSelector*>()
            .fromNode(ButtonSelector::create({
                { "Join", pJoinButtons },
                { "Host", pHostButtons },
            }))
            .move(0.0f, this->m_pLrSize.height / 2 - 60.0f)
            .anchor({ 0, 0 })
            .scale(.6f)
            .done()
    );

    this->m_pButtonMenu->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<ButtonSprite*>()
                    .fromNode(ButtonSprite::create(
                        "OK", 0, 0, "goldFont.fnt", "GJ_button_01.png", 0, .8f
                    ))
                    .scale(.8f)
                    .done(),
                this,
                (SEL_MenuHandler)&GoLiveLayer::onClose
            ))
            .move(0.0f, - this->m_pLrSize.height / 2 + 25.0f)
            .done()
    );
}

GoLiveLayer* GoLiveLayer::create() {
    auto pRet = new GoLiveLayer();

    if (pRet && pRet->init(320.0f, 240.0f, "GJ_square01.png", "Go Live")) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
