#include "ActionListView.hpp"

ActionCell::ActionCell(const char* name, CCSize size) :
    TableViewCell(name, size.width, size.height) {}

void ActionCell::draw() {
    // just call StatsCell::draw, no one will notice
    reinterpret_cast<void(__thiscall*)(ActionCell*)>(
        base + 0x59d40
    )(this);
}

void ActionCell::loadFromAction(ActionObject* action) {
    this->m_pAction = action;

    this->m_pLayer->setVisible(true);
    this->m_pBGLayer->setOpacity(255);
    
    auto cell = action->createActionCellItem();
    cell->setPosition(this->m_fWidth / 2, this->m_fHeight / 2);
    this->m_pLayer->addChild(cell);
}

ActionCell* ActionCell::create(const char* key, CCSize size) {
    auto pRet = new ActionCell(key, size);

    if (pRet) {
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}



void ActionListView::setupList() {
    this->m_fItemSeparation = 25.0f;

    if (!this->m_pEntries->count()) return;

    this->m_pTableView->reloadData();

    if (this->m_pEntries->count() == 1)
        this->m_pTableView->moveToTopWithOffset(this->m_fItemSeparation);
    
    this->m_pTableView->moveToTop();
}

TableViewCell* ActionListView::getListCell(const char* key) {
    return ActionCell::create(key, { this->m_fWidth, 55.0f });
}

void ActionListView::loadCell(TableViewCell* cell, unsigned int index) {
    as<ActionCell*>(cell)->loadFromAction(
        as<ActionObject*>(this->m_pEntries->objectAtIndex(index))
    );
    as<StatsCell*>(cell)->updateBGColor(index);
}

ActionListView* ActionListView::create(
    CCArray* actions,
    float width,
    float height
) {
    auto pRet = new ActionListView;

    if (pRet) {
        if (pRet->init(actions, kBoomListType_Action, width, height)) {
            pRet->autorelease();
            return pRet;
        }
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
