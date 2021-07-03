#include "GoLiveLayer.hpp"

using namespace gd;
using namespace gdmake;
using namespace gdmake::extra;
using namespace cocos2d;

void GoLiveLayer::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->m_pLayer->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(
                240.0f, "Server Address",
                "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.:-_?/0123456789~"_s
            ))
            .move(winSize / 2 + CCPoint { 0, 30.0f })
            .exec([](auto s) -> void {
                s->getInputNode()->setLabelPlaceholerScale(.5f);
            })
            .save(&this->m_pAddrInput)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(240.0f, "Room Name"))
            .move(winSize / 2 + CCPoint { 0, -10.0f })
            .exec([](auto s) -> void {
                s->getInputNode()->setLabelPlaceholerScale(.5f);
            })
            .save(&this->m_pRoomInput)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(240.0f, "Room Password (Optional)"))
            .move(winSize / 2 + CCPoint { 0, -50.0f })
            .exec([](auto s) -> void {
                s->getInputNode()->setLabelPlaceholerScale(.5f);
            })
            .save(&this->m_pRoomPWInput)
            .done()
    );

    this->m_pButtonMenu->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<ButtonSprite*>()
                    .fromNode(ButtonSprite::create(
                        "Host", 0, 0, "goldFont.fnt", "GJ_button_01.png", 0, .8f
                    ))
                    .scale(.8f)
                    .done(),
                this,
                (SEL_MenuHandler)&GoLiveLayer::onHost
            ))
            .move(0.0f, - this->m_pLrSize.height / 2 + 25.0f)
            .done()
    );
}

void GoLiveLayer::onHost(CCObject* pSender) {
    LiveManager::sharedState()->goLive(
        this->m_pAddrInput->getString(),
        this->m_pRoomInput->getString(),
        this->m_pRoomPWInput->getString()
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
