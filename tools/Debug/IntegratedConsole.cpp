#include "IntegratedConsole.hpp"

void IntegratedConsole::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->m_bNoElasticity = true;

    this->m_pBGSprite->setColor({ 150, 150, 150 });

    auto arr = CCArray::create();
    for (auto const& str : BetterEdit::internal_log()) {
        arr->addObject(CCString::create(str));
    }

    this->m_pList = DebugListView::create(arr, 400.f, 220.f);
    this->m_pList->setPosition(
        winSize / 2 - CCSize {
            this->m_pList->m_fWidth,
            this->m_pList->m_fHeight + 20.f
        } / 2
    );
    this->m_pList->m_pTableView->m_pContentLayer->setPositionY(0);
    this->addChild(this->m_pList);
}

IntegratedConsole* IntegratedConsole::create() {
    auto ret = new IntegratedConsole;

    if (ret && ret->init(440.f, 280.f, "GJ_square05.png", "Integrated Console")) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
