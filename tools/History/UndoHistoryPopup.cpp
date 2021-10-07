#include "UndoHistoryPopup.hpp"

UndoHistoryMode g_eMode;

void UndoHistoryPopup::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    
    this->m_bNoElasticity = true;

    this->m_eMode = g_eMode;

    auto gdModeBtnSpr = ButtonSprite::create(
        "GD", 0, 0, "goldFont.fnt", "GJ_button_01.png", 0, .9f
    );
    gdModeBtnSpr->setScale(.5f);
    auto gdModeBtnSpr2 = ButtonSprite::create(
        "GD", 0, 0, "goldFont.fnt", "GJ_button_02.png", 0, .9f
    );
    gdModeBtnSpr2->setScale(.5f);
    auto gdModeBtn = CCMenuItemToggler::create(
        gdModeBtnSpr, gdModeBtnSpr2,
        this, menu_selector(UndoHistoryPopup::onMode)
    );
    gdModeBtn->setTag(kUndoHistoryModeGD);
    this->m_pButtonMenu->addChild(gdModeBtn);
    this->m_vModeBtns.push_back(gdModeBtn);

    auto hardModeBtnSpr = ButtonSprite::create(
        "Full", 0, 0, "goldFont.fnt", "GJ_button_01.png", 0, .9f
    );
    hardModeBtnSpr->setScale(.5f);
    auto hardModeBtnSpr2 = ButtonSprite::create(
        "Full", 0, 0, "goldFont.fnt", "GJ_button_02.png", 0, .9f
    );
    hardModeBtnSpr2->setScale(.5f);
    auto hardModeBtn = CCMenuItemToggler::create(
        hardModeBtnSpr, hardModeBtnSpr2,
        this, menu_selector(UndoHistoryPopup::onMode)
    );
    hardModeBtn->setTag(kUndoHistoryModeHard);
    this->m_pButtonMenu->addChild(hardModeBtn);
    this->m_vModeBtns.push_back(hardModeBtn);

    this->m_pButtonMenu->alignItemsHorizontallyWithPadding(5.f);
    
    gdModeBtn->setPositionY(m_pLrSize.height / 2 - 45.f);
    hardModeBtn->setPositionY(m_pLrSize.height / 2 - 45.f);

    this->m_pInfoLabel = CCLabelBMFont::create("No History Found!", "bigFont.fnt");
    this->m_pInfoLabel->setScale(.6f);
    this->m_pInfoLabel->setPosition(winSize / 2);
    this->m_pLayer->addChild(this->m_pInfoLabel);

    this->updateList();
    this->updateButtons();
}

void UndoHistoryPopup::updateList() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    if (this->m_pList) {
        this->m_pList->removeFromParent();
    }

    this->m_pInfoLabel->setVisible(false);

    CCArray* history = nullptr;
    BoomListType type = kBoomListType_Action;
    
    switch (this->m_eMode) {
        case kUndoHistoryModeGD:
            history = this->m_pEditor->m_pUndoObjects;
            type = kBoomListType_UndoObject;
            break;

        case kUndoHistoryModeHard:
            history = UndoHistoryManager::get()->getActionHistory();
            break;
    }

    if (!history)
        return;

    if (!history->count()) {

        this->m_pInfoLabel->setVisible(true);

    } else {

        this->m_pList = ActionListView::create(
            history, type,
            280.0f, 160.0f
        );
        this->m_pList->setPosition(
            winSize / 2 - CCPoint { 140.f, 95.f }
        );
        this->m_pLayer->addChild(this->m_pList);

    }
}

void UndoHistoryPopup::updateButtons(CCObject* obj) {
    for (auto const& btn : this->m_vModeBtns) {
        if (btn != obj) {
            btn->toggle(this->m_eMode == btn->getTag());
        } else {
            btn->toggle(false);
        }
    }
}

void UndoHistoryPopup::onMode(CCObject* pSender) {
    this->m_eMode = static_cast<UndoHistoryMode>(pSender->getTag());
    this->updateList();
    this->updateButtons(pSender);
    g_eMode = this->m_eMode;
}

UndoHistoryPopup* UndoHistoryPopup::create(LevelEditorLayer* lel) {
    auto ret = new UndoHistoryPopup;

    if (
        ret &&
        ((ret->m_pEditor = lel) || true) &&
        ret->init(340.0f, 260.0f, "GJ_square01.png", "Undo History")
    ) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

void UndoHistoryPopup::popup(LevelEditorLayer* lel) {
    if (BetterEdit::useExperimentalFeatures([lel](void) -> void {
        UndoHistoryPopup::create(lel)->show();
    })) {
        UndoHistoryPopup::create(lel)->show();
    }
}
