#include "UndoHistoryPopup.hpp"

void UndoHistoryPopup::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    
    this->m_bNoElasticity = true;

    auto history = UndoHistoryManager::get()->getActionHistory();

    if (!history->count()) {

        auto label = CCLabelBMFont::create("No History Found!", "bigFont.fnt");
        label->setScale(.6f);
        label->setPosition(winSize / 2);
        this->m_pLayer->addChild(label);

    } else {

        this->m_pList = ActionListView::create(
            history,
            280.0f, 180.0f
        );
        this->m_pList->setPosition(winSize / 2 - this->m_pList->getScaledContentSize() / 2);
        this->m_pLayer->addChild(this->m_pList);

    }
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
