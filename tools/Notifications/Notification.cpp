#include "Notification.hpp"
#include "../FLAlertLayerFix/FLAlertLayerFix.hpp"

const float Notification::s_fNotificationHeight = 30.f;

bool Notification::init(
    NotificationType type,
    std::string const& msg,
    bool hideAuto,
    Click cb,
    CCNode* target
) {
    if (!CCNode::init())
        return false;
    
    this->m_bHideAutomatically = hideAuto;
    this->m_notificationClicked = cb;
    this->m_pTarget = target;

    CCSize const size = { 150.f, s_fNotificationHeight };

    this->m_pBGSprite = CCScale9Sprite::create(
        "GJ_square02.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );
    this->m_pBGSprite->setScale(.5f);
    this->m_pBGSprite->setContentSize(size * 2);
    this->m_pBGSprite->setPosition(size / 2);
    this->addChild(this->m_pBGSprite);

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
    if (msg.size()) {
        auto text = CCLabelBMFont::create(msg.c_str(), "chatFont.fnt");
        text->setPosition({ size.height, size.height / 2 });
        text->setAnchorPoint({ .0f, .5f });
        text->setScale(.5f);
        // text->setColor(cc3x(0x33210b));
        this->addChild(text);
    }

    auto closeSpr = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
    closeSpr->setScale(.4f);

    this->m_pCloseBtn = CCMenuItemSpriteExtra::create(
        closeSpr, this, menu_selector(Notification::onHide)
    );
    this->m_pCloseBtn->setPosition(0, s_fNotificationHeight);
    this->m_pCloseBtn->setVisible(false);
    this->addChild(this->m_pCloseBtn);

    this->setSuperMouseHitOffset(size / 2);
    this->setSuperMouseHitSize(size);

    this->setContentSize(size);

    return true;
}

void Notification::waitToHide() {
    if (this->m_bHideAutomatically) {
        this->runAction(CCSequence::create(
            CCDelayTime::create(m_fHideTime),
            CCEaseExponentialIn::create(
                CCMoveBy::create(.6f, { this->getScaledContentSize().width, 0 })
            ),
            CCCallFunc::create(this, callfunc_selector(Notification::hide)),
            nullptr
        ));
    }
}

void Notification::onHide(CCObject*) {
    this->hide();
}

void Notification::show() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    CCNode* target = CCDirector::sharedDirector()->getRunningScene();
    if (this->m_pTarget) {
        target = this->m_pTarget;
    }
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
    target->addChild(this);
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

void Notification::mouseMoveSuper(CCPoint const& mpos) {
    this->m_pBGSprite->setColor(cc3x(0xa));
    this->m_pCloseBtn->setVisible(true);
    if (!nodeIsHovered(this->m_pCloseBtn, mpos)) {
        this->m_pCloseBtn->unselected();
    }
}

void Notification::mouseLeaveSuper(CCPoint const&) {
    this->m_pBGSprite->setColor(cc3x(0xf));
    this->m_pBGSprite->setOpacity(255);
    this->m_pCloseBtn->setVisible(false);
}

bool Notification::mouseDownSuper(MouseButton btn, CCPoint const& mpos) {
    if (btn == kMouseButtonLeft) {
        if (nodeIsHovered(this->m_pCloseBtn, mpos)) {
            this->m_pCloseBtn->selected();
        } else {
            this->m_pBGSprite->setOpacity(150);
        }
    }
    return true;
}

bool Notification::mouseUpSuper(MouseButton btn, CCPoint const& mpos) {
    if (btn == kMouseButtonLeft) {
        if (nodeIsHovered(this->m_pCloseBtn, mpos)) {
            this->m_pCloseBtn->activate();
        } else {
            if (this->m_notificationClicked) {
                this->m_notificationClicked();
            }
            this->m_pBGSprite->setOpacity(255);
            this->hide();
            SuperMouseManager::get()->popDelegate(this);
        }
    }
    return true;
}

Notification* Notification::create(
    NotificationType type,
    std::string const& msg,
    bool hideAuto,
    Click cb,
    CCNode* target
) {
    auto ret = new Notification;

    if (ret && ret->init(type, msg, hideAuto, cb, target)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

Notification* Notification::create(
    NotificationType type,
    std::string const& msg,
    Click cb,
    CCNode* target
) {
    return create(type, msg, false, cb);
}
