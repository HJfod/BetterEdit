#include "UndoHistoryPopup.hpp"

void UndoHistoryPopup::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    
    this->m_bNoElasticity = true;

    // this->m_pList = ActionListView::create(
    //     UndoHistoryManager::get()->getActionHistory(),
    //     280.0f, 200.0f
    // );
    // this->m_pList->setPosition(winSize / 2);
    // this->addChild(this->m_pList);
}

UndoHistoryPopup* UndoHistoryPopup::create() {
    auto ret = new UndoHistoryPopup;

    if (ret && ret->init(340.0f, 260.0f, "GJ_square01.png", "Undo History")) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
