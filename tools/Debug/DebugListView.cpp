#include "DebugListView.hpp"

DebugCell::DebugCell(const char* name, CCSize size) :
    TableViewCell(name, size.width, size.height) {}

void DebugCell::draw() {
    // just call StatsCell::draw, no one will notice
    reinterpret_cast<void(__thiscall*)(DebugCell*)>(
        base + 0x59d40
    )(this);
}

void DebugCell::loadFromString(CCString* str) {
    this->m_pLayer->setVisible(true);
    this->m_pBGLayer->setOpacity(255);
    
    auto cell = CCLabelBMFont::create(str->getCString(), "chatFont.fnt");
    cell->setAnchorPoint({ .0f, .5f });
    cell->setPosition(this->m_fHeight / 2, this->m_fHeight / 2);
    this->m_pLayer->addChild(cell);
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
    this->m_fItemSeparation = 20.0f;

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
    as<StatsCell*>(cell)->updateBGColor(index);
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
