#include "LayerViewPopup.hpp"

int g_nPage = 0;

void LayerViewPopup::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->m_bNoElasticity = true;
    this->m_nCurrentPage = g_nPage;

    this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
        .fromNode(CCMenuItemSpriteExtra::create(
            CCNodeConstructor()
                .fromFrameName("GJ_arrow_01_001.png")
                .scale(.8f)
                .done(),
            this,
            (SEL_MenuHandler)&LayerViewPopup::onPage
        ))
        .udata(-1)
        .move(- this->m_pLrSize.width / 2 + 25.0f, 0.0f)
        .done()
    );
    this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
        .fromNode(CCMenuItemSpriteExtra::create(
            CCNodeConstructor()
                .fromFrameName("GJ_arrow_01_001.png")
                .scale(.8f)
                .flipX()
                .done(),
            this,
            (SEL_MenuHandler)&LayerViewPopup::onPage
        ))
        .udata(1)
        .move(this->m_pLrSize.width / 2 - 25.0f, 0.0f)
        .done()
    );
    this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
        .fromNode(CCMenuItemSpriteExtra::create(
            CCNodeConstructor<ButtonSprite*>()
                .fromNode(ButtonSprite::create(
                    "Lock All", 80, true, "bigFont.fnt", "GJ_button_04.png", 0, .7f
                ))
                .scale(.5f)
                .done(),
            this,
            nullptr
        ))
        .move(-this->m_pLrSize.width / 2 + 45.0f, this->m_pLrSize.height / 2 - 24.0f)
        .done()
    );
    this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
        .fromNode(CCMenuItemSpriteExtra::create(
            CCNodeConstructor<ButtonSprite*>()
                .fromNode(ButtonSprite::create(
                    "Unlock All", 80, true, "bigFont.fnt", "GJ_button_04.png", 0, .7f
                ))
                .scale(.5f)
                .done(),
            this,
            nullptr
        ))
        .move(-this->m_pLrSize.width / 2 + 105.0f, this->m_pLrSize.height / 2 - 24.0f)
        .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<CCLabelBMFont*>()
        .fromText("1", "goldFont.fnt")
        .move(winSize / 2 + this->m_pLrSize / 2 - CCPoint { 30.0f, 20.0f })
        .scale(.7f)
        .save(&m_pPageLabel)
        .done()
    );

    this->updatePage();
}

void LayerViewPopup::createItemAtPosition(float x, float y, int index) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto ixLabel = CCLabelBMFont::create(std::to_string(index).c_str(), "goldFont.fnt");

    ixLabel->setPositionX(x + winSize.width / 2);
    ixLabel->setPositionY(y + winSize.height / 2 + 20.0f);
    ixLabel->limitLabelWidth(40.0f, .5f, .2f);

    this->m_pLayer->addChild(ixLabel);
    this->m_vPageContent.push_back(ixLabel);

    auto lock = 
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor()
                    .fromFrameName("GJ_lock_open_001.png")
                    .scale(.7f)
                    .done(),
                this,
                nullptr
            ))
            .move(x, y)
            .done();

    this->m_pButtonMenu->addChild(lock);
    this->m_vPageContent.push_back(lock);
}

void LayerViewPopup::updatePage() {
    for (auto const& node : this->m_vPageContent)
        node->removeFromParentAndCleanup(true);
    
    this->m_vPageContent.clear();

    for (int i = 0; i < page_max_item_count; i++) {
        if (i + page_max_item_count * this->m_nCurrentPage > LayerManager::max_layer_num)
            continue;

        float w = this->m_pLrSize.width - 150.0f, h = this->m_pLrSize.height - 140.0f;
        int row_count = page_max_item_count / page_row_item_count;

        float posX = static_cast<float>(i % page_row_item_count) / (page_row_item_count - 1) * w - w / 2;
        float posY = (static_cast<float>(row_count - 1 - floor(i / page_row_item_count)) / (row_count - 1)) * h - h / 2;

        this->createItemAtPosition(posX, posY, i + page_max_item_count * this->m_nCurrentPage);
    }
    
    this->m_pPageLabel->setString(std::to_string(this->m_nCurrentPage + 1).c_str());
}

void LayerViewPopup::onPage(CCObject* pSender) {
    this->m_nCurrentPage += as<int>(as<CCNode*>(pSender)->getUserData());

    if (this->m_nCurrentPage < 0)
        this->m_nCurrentPage = 0;
    if (this->m_nCurrentPage > LayerManager::max_layer_num / page_max_item_count)
        this->m_nCurrentPage = LayerManager::max_layer_num / page_max_item_count;
    
    this->updatePage();
}

void LayerViewPopup::onClose(CCObject* pSender) {
    BrownAlertDelegate::onClose(pSender);
}

LayerViewPopup* LayerViewPopup::create() {
    auto pRet = new LayerViewPopup();

    if (pRet && pRet->init(460.0f, 260.0f, "GJ_square01.png", "Editor Layers")) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
