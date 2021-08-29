#include "Scrollbar.hpp"
#include "../EyeDropper/eyeDropper.hpp"

bool Scrollbar::ccTouchBegan(CCTouch* touch, CCEvent*) {
    m_pTouch = touch;
    m_pTouch->retain();

    return false;
}

void Scrollbar::ccTouchMoved(CCTouch*, CCEvent*) {

}

void Scrollbar::ccTouchEnded(CCTouch*, CCEvent*) {
    m_pTouch->release();
    m_pTouch = nullptr;
}

void Scrollbar::ccTouchCancelled(CCTouch*, CCEvent*) {
    m_pTouch->release();
    m_pTouch = nullptr;
}

void Scrollbar::visit() {
    CCLayer::visit();

    this->draw();
}

void Scrollbar::draw() {
    CCLayer::draw();

    this->m_pBG->setContentSize({
        this->m_fWidth, this->m_pList->m_fHeight
    });
    this->m_pBG->setScale(1.0f);
    this->m_pBG->setColor({ 0, 0, 0 });
    this->m_pBG->setOpacity(150);
    this->m_pBG->setPosition(0.0f, 0.0f);

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto winSizePx = CCDirector::sharedDirector()->getOpenGLView()->getViewPortRect();
    auto ratio_w = winSize.width / winSizePx.size.width;
    auto ratio_h = winSize.height / winSizePx.size.height;
    auto mpos = CCDirector::sharedDirector()->getOpenGLView()->getMousePosition();
    mpos.y = winSizePx.size.height - mpos.y;
    mpos.x *= ratio_w;
    mpos.y *= ratio_h;

    auto pos = this->convertToNodeSpace(mpos);

    auto h = this->m_pList->m_pTableView->m_pContentLayer->getScaledContentSize().height
        - this->m_pList->m_fHeight + this->m_pList->m_pTableView->m_fScrollLimitTop;
    auto p = this->m_pList->m_fHeight /
        this->m_pList->m_pTableView->m_pContentLayer->getScaledContentSize().height;

    auto trackHeight = min(p, 1.0f) * this->m_pList->m_fHeight / .4f;
    auto trackPosY = this->m_pTrack->getPositionY();

    auto o = 100;

    if (CCRect {
        - this->m_fWidth / 2,
        - this->m_pList->m_fHeight / 2,
        this->m_pBG->getScaledContentSize().width,
        this->m_pBG->getScaledContentSize().height
    }.containsPoint(pos)) {
        o = 160;

        if (isLeftMouseButtonPressed())
            m_bScrolling = true;
    }
    
    if (!isLeftMouseButtonPressed())
        m_bScrolling = false;

    if (m_bScrolling) {
        auto posY = h * (
            (-pos.y - this->m_pList->m_fHeight / 2 + trackHeight / 4 - 5) /
            (this->m_pList->m_fHeight - trackHeight / 2 + 10)
        );

        if (posY > 0.0f)
            posY = 0.0f;
        
        if (posY < -h)
            posY = -h;

        this->m_pList->m_pTableView->m_pContentLayer->setPositionY(posY);

        o = 255;
    }

    this->m_pTrack->setScale(.4f);
    this->m_pTrack->setColor({ 255, 255, 255 });
    this->m_pTrack->setOpacity(o);


    auto y = this->m_pList->m_pTableView->m_pContentLayer->getPositionY();

    trackPosY = - this->m_pList->m_fHeight / 2 + trackHeight / 4 - 5.0f + 
        ((-y) / h) * (this->m_pList->m_fHeight - trackHeight / 2 + 10.0f);

    auto fHeightTop = [&]() -> float {
        return trackPosY - this->m_pList->m_fHeight / 2 + trackHeight * .4f / 2 + 3.0f;
    };
    auto fHeightBottom = [&]() -> float {
        return trackPosY + this->m_pList->m_fHeight / 2 - trackHeight * .4f / 2 - 3.0f;
    };
    
    if (fHeightTop() > 0.0f) {
        trackHeight -= fHeightTop();
        trackPosY -= fHeightTop();
    }
    
    if (fHeightBottom() < 0.0f) {
        trackHeight += fHeightBottom();
        trackPosY -= fHeightBottom();
    }

    this->m_pTrack->setPosition(0.0f, trackPosY);

    this->m_pTrack->setContentSize({
        this->m_fWidth, trackHeight
    });
}

void Scrollbar::setList(CustomListView* list) {
    this->m_pList = list;
}

bool Scrollbar::init(CustomListView* list) {
    if (!this->CCLayer::init())
        return false;
    
    this->setList(list);
    this->m_fWidth = 8.0f;

    this->m_pBG = CCScale9Sprite::createWithSpriteFrameName(
        "BE_scrollbar.png", { 0.0f, 5.0f, 10.0f, 10.0f }
    );
    if (!this->m_pBG)
        this->m_pBG = CCScale9Sprite::create(
            "square02_small.png", { 0.0f, 0.0f, 40.0f, 40.0f }
        );
    this->addChild(this->m_pBG);

    this->m_pTrack = CCScale9Sprite::createWithSpriteFrameName(
        "BE_scrollbar.png", { 0.0f, 5.0f, 10.0f, 10.0f }
    );
    if (!this->m_pTrack)
        this->m_pTrack = CCScale9Sprite::create(
            "square02_small.png", { 0.0f, 0.0f, 40.0f, 40.0f }
        );
    this->addChild(this->m_pTrack);

    this->m_pTouch = nullptr;
    this->setTouchEnabled(true);
    this->registerWithTouchDispatcher();
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 50, true);
    CCDirector::sharedDirector()->getTouchDispatcher()->incrementForcePrio(2);

    return true;
}

Scrollbar* Scrollbar::create(CustomListView* list) {
    auto ret = new Scrollbar;

    if (ret && ret->init(list)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
