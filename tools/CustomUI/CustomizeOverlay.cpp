#include "CustomizeOverlay.hpp"

std::ostream& operator<<(std::ostream& stream, CCPoint const& pos) {
    return stream << pos.x << ", " << pos.y;
}

std::ostream& operator<<(std::ostream& stream, CCSize const& pos) {
    return stream << pos.width << " : " << pos.height;
}

CCSize operator-(CCSize const& size) {
    return { -size.width, -size.height };
}

void CustomizeArea::updateSize() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    bool first = true;
    float x, y, w, h;
    CCARRAY_FOREACH_B_TYPE(this->m_pItems, node, CCNode) {
        auto pos = node->getParent()->convertToWorldSpace(
            node->getPosition()
        );
        auto size = node->getScaledContentSize();

        if (first || pos.x - size.width / 2 < x)
            x = pos.x - size.width / 2;
        if (first || pos.x + size.width / 2 > w)
            w = pos.x + size.width / 2;
        if (first || pos.y - size.height / 2 < y)
            y = pos.y - size.height / 2;
        if (first || pos.y + size.height / 2 > h)
            h = pos.y + size.height / 2;
            
        first = false;
    }

    w -= x;
    h -= y;

    this->setPosition(x + w / 2, y + h / 2);
    this->setContentSize({ w, h });
}

bool CustomizeArea::init(CCArray* items) {
    if (!CCLayer::init())
        return false;
    
    this->m_pItems = items;
    this->m_pItems->retain();

    return true;
}

void CustomizeArea::draw() {
    CCLayer::draw();

    this->updateSize();

    auto size = this->getScaledContentSize();

    ccDrawColor4B(
        0, 255, this->m_bSuperMouseHovered * 255,
        this->m_bSuperMouseHovered ? 255 : 120
    );
    ccPointSize(4);
    ccDrawRect(-size / 2, size / 2);
}

CustomizeArea::~CustomizeArea() {
    this->m_pItems->release();
}

CustomizeArea* CustomizeArea::create(CCArray* items) {
    auto ret = new CustomizeArea;

    if (ret && ret->init(items)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}



bool CustomizeOverlay::init(EditorUI* ui) {
    if (!CCLayerColor::initWithColor({ 0, 0, 0, 200 }))
        return false;

    this->m_pUI = ui;

    auto sideBtns = CCArray::create();

    sideBtns->addObject(ui->m_pCopyBtn);
    sideBtns->addObject(ui->m_pPasteBtn);
    sideBtns->addObject(ui->m_pCopyPasteBtn);
    sideBtns->addObject(ui->m_pEditSpecialBtn);
    sideBtns->addObject(ui->m_pEditGroupBtn);
    sideBtns->addObject(ui->m_pEditObjectBtn);
    sideBtns->addObject(ui->m_pCopyValuesBtn);
    sideBtns->addObject(ui->m_pPasteStateBtn);
    sideBtns->addObject(ui->m_pPasteColorBtn);
    sideBtns->addObject(ui->m_pGoToLayerBtn);

    this->m_pAreaSideBtns = CustomizeArea::create(sideBtns);
    this->addChild(this->m_pAreaSideBtns);

    return true;
}

CustomizeOverlay* CustomizeOverlay::create(EditorUI* ui) {
    auto ret = new CustomizeOverlay;

    if (ret && ret->init(ui)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
