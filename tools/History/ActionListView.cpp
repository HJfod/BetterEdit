#include "ActionListView.hpp"

std::string UndoCommandToString(UndoCommand u) {
    switch (u) {
        case kUndoCommandNew:       return "New";
        case kUndoCommandPaste:     return "Paste";
        case kUndoCommandDelete:    return "Delete";
        case kUndoCommandTransform: return "Transform";
        case kUndoCommandSelect:    return "Select";
        default:                    return "Unknown";
    }
};

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
    
    auto cell = action->createActionCellItem(this->m_fWidth, this->m_fHeight);
    this->m_pLayer->addChild(cell);
}

void ActionCell::loadFromUndoObject(UndoObject* obj) {
    this->m_pUndoObject = obj;

    this->m_pLayer->setVisible(true);
    this->m_pBGLayer->setOpacity(255);

    auto actionStr = UndoCommandToString(obj->m_eCommand);

    auto label = CCLabelBMFont::create(actionStr.c_str(), "bigFont.fnt");
    label->setScale(.5f);
    label->setAnchorPoint({ .0f, .5f });
    label->setPosition(this->m_fHeight / 2, this->m_fHeight / 2);
    this->m_pLayer->addChild(label);

    auto idLabel = CCLabelBMFont::create(
        std::to_string(obj->m_eCommand).c_str(),
        "bigFont.fnt"
    );
    idLabel->setScale(.5f);
    idLabel->setAnchorPoint({ 1.f, .5f });
    idLabel->setPosition(
        this->m_fWidth - this->m_fHeight / 2,
        this->m_fHeight / 2
    );
    this->m_pLayer->addChild(idLabel);
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
    this->m_fItemSeparation = 20.0f;

    if (!this->m_pEntries->count()) return;

    this->m_pTableView->reloadData();

    if (this->m_pEntries->count() == 1)
        this->m_pTableView->moveToTopWithOffset(this->m_fItemSeparation);
    
    this->m_pTableView->moveToTop();
}

TableViewCell* ActionListView::getListCell(const char* key) {
    return ActionCell::create(key, { this->m_fWidth, this->m_fItemSeparation });
}

void ActionListView::loadCell(TableViewCell* cell, unsigned int index) {
    switch (this->m_eType) {
        case kBoomListType_UndoObject:
            as<ActionCell*>(cell)->loadFromUndoObject(
                as<UndoObject*>(this->m_pEntries->objectAtIndex(index))
            );
            break;
        
        case kBoomListType_Action:
            as<ActionCell*>(cell)->loadFromAction(
                as<ActionObject*>(this->m_pEntries->objectAtIndex(index))
            );
            break;
    }
    as<StatsCell*>(cell)->updateBGColor(index);
}

ActionListView* ActionListView::create(
    CCArray* actions,
    BoomListType type,
    float width,
    float height
) {
    auto pRet = new ActionListView;

    if (pRet) {
        if (pRet->init(actions, type, width, height)) {
            pRet->autorelease();
            return pRet;
        }
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
