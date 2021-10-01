#include "DebugListView.hpp"

DebugCell::DebugCell(const char* name, CCSize size) :
    TableViewCell(name, size.width, size.height) {}

void DebugCell::draw() {
    // just call StatsCell::draw, no one will notice
    reinterpret_cast<void(__thiscall*)(DebugCell*)>(
        base + 0x59d40
    )(this);
}

void DebugCell::updateBGColor(int ix) {
    this->m_pBGLayer->setOpacity(ix % 2 ? 150 : 50);
    this->m_pBGLayer->setColor({ 0, 0, 0 });
}

void DebugCell::loadFromString(CCString* str) {
    this->m_pLayer->setVisible(true);
    this->m_pBGLayer->setOpacity(255);
    
    // auto cell = CCLabelTTF::create(str->getCString(), "Consolas", 18.f);
    auto cell = CCLabelBMFont::create(str->getCString(), "chatFont.fnt");
    // cell->setAnchorPoint({ .0f, .75f });
    cell->setAnchorPoint({ .0f, .5f });
    cell->setPosition({
        this->m_fHeight / 2,
        this->m_fHeight / 2
    });
    cell->setScale(.5f);
    this->m_pLayer->addChild(cell);

    this->m_pLabel = cell;

    this->updateLabelColor();
}

void DebugCell::updateLabelColor() {
    auto str = this->m_pLabel->getString();
    ccColor3B col;
    CCARRAY_FOREACH_B_BASE(this->m_pLabel->getChildren(), child, CCSprite*, ix) {
        switch (str[ix]) {
            case '[': col = { 100, 100, 100 }; break;
        }
        child->setColor(col);
        switch (str[ix]) {
            case ']': col = { 255, 255, 255 }; break;
        }
    }
}

DebugCell* DebugCell::create(const char* key, CCSize size) {
    auto pRet = new DebugCell(key, size);

    if (pRet) {
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}



void DebugListView::setupList() {
    this->m_fItemSeparation = 16.0f;

    if (!this->m_pEntries->count()) return;

    this->m_pTableView->reloadData();

    if (this->m_pEntries->count() == 1)
        this->m_pTableView->moveToTopWithOffset(this->m_fItemSeparation);
    
    this->m_pTableView->moveToTop();
}

TableViewCell* DebugListView::getListCell(const char* key) {
    return DebugCell::create(key, { this->m_fWidth, this->m_fItemSeparation });
}

void DebugListView::loadCell(TableViewCell* cell, unsigned int index) {
    as<DebugCell*>(cell)->loadFromString(
        as<CCString*>(this->m_pEntries->objectAtIndex(index))
    );
    as<DebugCell*>(cell)->updateBGColor(index);
}

DebugListView* DebugListView::create(
    CCArray* actions,
    float width,
    float height
) {
    auto pRet = new DebugListView;

    if (pRet) {
        if (pRet->init(actions, kBoomListType_Debug, width, height)) {
            pRet->autorelease();
            return pRet;
        }
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
