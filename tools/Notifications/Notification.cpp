#include "Notification.hpp"
#include "../FLAlertLayerFix/FLAlertLayerFix.hpp"

const float Notification::s_fNotificationHeight = 30.f;

bool Notification::init(NotificationType type, const char* msg) {
    if (!CCNode::init())
        return false;

    CCSize const size = { 150.f, s_fNotificationHeight };

    auto bg = CCScale9Sprite::create(
        "GJ_square02.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );
    bg->setScale(.5f);
    bg->setContentSize(size * 2);
    bg->setPosition(size / 2);
    this->addChild(bg);

    const char* sprName = nullptr;
    float sprScale = .5f;
    switch (type) {
        case kNotificationTypeInfo:
            sprName = "GJ_infoIcon_001.png";
            break;
        case kNotificationTypeError:
            sprName = "exMark_001.png";
            sprScale = .3f;
            break;
    }
    if (sprName) {
        auto spr = CCSprite::createWithSpriteFrameName(sprName);
        spr->setPosition({ size.height / 2, size.height / 2 });
        spr->setScale(sprScale);
        this->addChild(spr);
    }
    if (msg) {
        auto text = CCLabelBMFont::create(msg, "chatFont.fnt");
        text->setPosition({ size.height, size.height / 2 });
        text->setAnchorPoint({ .0f, .5f });
        text->setScale(.5f);
        // text->setColor(cc3x(0x33210b));
        this->addChild(text);
    }

    this->setContentSize(size);

    return true;
}

void Notification::waitToHide() {
    this->runAction(CCSequence::create(
        CCDelayTime::create(2.f),
        CCEaseExponentialIn::create(
            CCMoveBy::create(.6f, { this->getScaledContentSize().width, 0 })
        ),
        CCCallFunc::create(this, callfunc_selector(Notification::hide)),
        nullptr
    ));
}

void Notification::show() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    this->setPosition(
        winSize.width + this->getScaledContentSize().width / 2,
        10.f
    );
    this->runAction(CCSequence::create(
        CCEaseExponentialOut::create(
            CCMoveBy::create(.3f, { -this->getScaledContentSize().width * 1.5f - 10.f, 0 })
        ),
        CCCallFunc::create(this, callfunc_selector(Notification::waitToHide)),
        nullptr
    ));
    scene->addChild(this);
    fixLayering(this);
    this->setZOrder(this->getZOrder() + 1000);
}

void Notification::hide() {
    this->m_bHiding = true;
    if (this->m_pDelegate) {
        this->m_pDelegate->notificationClosed(this);
    }
    this->removeFromParent();
}

bool Notification::isHiding() {
    return this->m_bHiding;
}

void Notification::setDelegate(NotificationDelegate* delegate) {
    this->m_pDelegate = delegate;
}

Notification* Notification::create(NotificationType type, const char* msg) {
    auto ret = new Notification;

    if (ret && ret->init(type, msg)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
