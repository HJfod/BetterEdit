#include "LayerViewPopup.hpp"
#include "LayerEditPopup.hpp"

int g_nPage = 0;

void LayerViewPopup::countObjectsPerLayer() {
    auto objs = LevelEditorLayer::get()->getAllObjects();

    this->m_mObjectsPerLayer = {};

    CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject)
        this->m_mObjectsPerLayer[obj->m_nEditorLayer]++;
        // this->m_mObjectsPerLayer[obj->m_nEditorLayer2]++;
}

void LayerViewPopup::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->countObjectsPerLayer();

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
                    "Lock All", 80, true, "bigFont.fnt", "GJ_button_04.png", 0, .9f
                ))
                .scale(.55f)
                .done(),
            this,
            (SEL_MenuHandler)&LayerViewPopup::onLockAll
        ))
        .udata(true)
        .move(-this->m_pLrSize.width / 2 + 55.0f, -this->m_pLrSize.height / 2 + 24.0f)
        .done()
    );
    this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
        .fromNode(CCMenuItemSpriteExtra::create(
            CCNodeConstructor<ButtonSprite*>()
                .fromNode(ButtonSprite::create(
                    "Unlock All", 80, true, "bigFont.fnt", "GJ_button_04.png", 0, .9f
                ))
                .scale(.55f)
                .done(),
            this,
            (SEL_MenuHandler)&LayerViewPopup::onLockAll
        ))
        .udata(false)
        .move(-this->m_pLrSize.width / 2 + 120.0f, -this->m_pLrSize.height / 2 + 24.0f)
        .done()
    );
    this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
        .fromNode(CCMenuItemSpriteExtra::create(
            CCNodeConstructor<ButtonSprite*>()
                .fromNode(ButtonSprite::create(
                    "Show All", 80, true, "bigFont.fnt", "GJ_button_04.png", 0, .9f
                ))
                .scale(.55f)
                .done(),
            this,
            (SEL_MenuHandler)&LayerViewPopup::onShowAll
        ))
        .udata(true)
        .move(this->m_pLrSize.width / 2 - 55.0f, -this->m_pLrSize.height / 2 + 24.0f)
        .done()
    );
    this->m_pButtonMenu->addChild(CCNodeConstructor<CCMenuItemSpriteExtra*>()
        .fromNode(CCMenuItemSpriteExtra::create(
            CCNodeConstructor<ButtonSprite*>()
                .fromNode(ButtonSprite::create(
                    "Hide All", 80, true, "bigFont.fnt", "GJ_button_04.png", 0, .9f
                ))
                .scale(.55f)
                .done(),
            this,
            (SEL_MenuHandler)&LayerViewPopup::onShowAll
        ))
        .udata(false)
        .move(this->m_pLrSize.width / 2 - 120.0f, -this->m_pLrSize.height / 2 + 24.0f)
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

void LayerViewPopup::updateLockSprite(CCMenuItemSpriteExtra* btn, LayerManager::Layer* layer) {
    btn->setNormalImage(
        CCNodeConstructor()
            .fromFrameName(layer->m_bLocked ? 
                "GJ_lock_001.png" :
                "GJ_lock_open_001.png")
            .scale(.7f)
            .move(btn->getNormalImage()->getPosition())
            .alpha(layer ? 255 : 120)
            .done()
    );
    if (!layer->m_bLocked)
        btn->getNormalImage()->setAnchorPoint({ 0.32f, 0.0f });
    else
        btn->getNormalImage()->setAnchorPoint({ 0.30f, 0.2f });
}

void LayerViewPopup::updateShowSprite(CCMenuItemSpriteExtra* btn, LayerManager::Layer* layer) {
    auto spr = btn->getNormalImage();
    spr->retain();

    btn->setNormalImage(
        CCNodeConstructor()
            .fromFile(layer->m_bVisible ?
                "BE_eye-on-thick.png" : 
                "BE_eye-off.png")
            .scale(spr->getScale())
            .csize(spr->getContentSize())
            .done()
    );

    btn->getNormalImage()->setPosition(spr->getPosition());
    btn->getNormalImage()->setAnchorPoint(spr->getAnchorPoint());
    btn->getNormalImage()->setScale(spr->getScale());

    spr->release();
}

void LayerViewPopup::updateLayerItem(int number, LayerManager::Layer* layer) {
    auto btn = as<CCMenuItemSpriteExtra*>(
        this->m_pButtonMenu->getChildByTag(
            LayerViewPopup::lock_tag_offset + number
        )
    );
    if (btn) this->updateLockSprite(btn, layer);

    auto name = as<CCLabelBMFont*>(
        this->m_pLayer->getChildByTag(
            LayerViewPopup::name_tag_offset + number
        )
    );
    if (name) {
        if (layer->m_sName.size())
            name->setString((std::to_string(number) + ": " + layer->m_sName).c_str());
        else
            name->setString(std::to_string(number).c_str());

        name->limitLabelWidth(45.0f, .3f, .1f);
    }

    auto view = as<CCMenuItemSpriteExtra*>(
        this->m_pButtonMenu->getChildByTag(
            LayerViewPopup::view_tag_offset + number
        )
    );
    if (view) this->updateShowSprite(view, layer);
}

void LayerViewPopup::createItemAtPosition(float x, float y, int index) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto bg = CCScale9Sprite::create(
        "square02c_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );

    bg->setPosition(CCPoint { x, y });
    bg->setScale(.5f);
    bg->setColor({ 0, 0, 0 });
    bg->setOpacity(75);
    bg->setContentSize({ 100.0f, 140.0f });

    this->m_pButtonMenu->addChild(bg);
    this->m_vPageContent.push_back(bg);

    auto ixLabel = CCLabelBMFont::create(std::to_string(index).c_str(), "bigFont.fnt");

    ixLabel->setPositionX(x + winSize.width / 2);
    ixLabel->setPositionY(y + winSize.height / 2 + 27.5f);
    ixLabel->limitLabelWidth(45.0f, .3f, .1f);
    ixLabel->setTag(name_tag_offset + index);

    this->m_pLayer->addChild(ixLabel);
    this->m_vPageContent.push_back(ixLabel);

    auto lock = 
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor()
                    .fromFrameName("GJ_lock_001.png")
                    .scale(.7f)
                    .done(),
                this,
                (SEL_MenuHandler)&LayerViewPopup::onLock
            ))
            .move(x - 10.0f, y + 8.0f)
            .udata(index)
            .tag(lock_tag_offset + index)
            .done();

    this->m_pButtonMenu->addChild(lock);
    this->m_vPageContent.push_back(lock);

    auto viewToggle = 
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor()
                    .fromFile("BE_eye-on.png")
                    .scale(.15f)
                    .exec([](auto t) -> void {
                        t->setContentSize(t->getScaledContentSize());
                    })
                    .done(),
                this,
                (SEL_MenuHandler)&LayerViewPopup::onShow
            ))
            .udata(index)
            .move(x + 10.0f, y + 8.0f)
            .tag(view_tag_offset + index)
            .done();

    this->m_pButtonMenu->addChild(viewToggle);
    this->m_vPageContent.push_back(viewToggle);

    auto gotoBtn = 
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor()
                    .fromFrameName("GJ_goToLayerBtn_001.png")
                    .scale(.5f)
                    .done(),
                this,
                (SEL_MenuHandler)&LayerViewPopup::onGoToLayer
            ))
            .move(x - 10.0f, y - 12.0f)
            .udata(index)
            .done();

    this->m_pButtonMenu->addChild(gotoBtn);
    this->m_vPageContent.push_back(gotoBtn);

    auto settings = 
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor()
                    .fromFrameName("GJ_editObjBtn4_001.png")
                    .scale(.5f)
                    .done(),
                this,
                (SEL_MenuHandler)&LayerViewPopup::onSettings
            ))
            .udata(index)
            .move(x + 10.0f, y - 12.0f)
            .done();

    this->m_pButtonMenu->addChild(settings);
    this->m_vPageContent.push_back(settings);

    auto objCountLabel = CCLabelBMFont::create((std::to_string(
        this->m_mObjectsPerLayer[index]
    ) + " objs").c_str(), "goldFont.fnt");

    objCountLabel->setPositionX(x + winSize.width / 2);
    objCountLabel->setPositionY(y + winSize.height / 2 - 27.5f);
    objCountLabel->limitLabelWidth(45.0f, .3f, .1f);

    this->m_pLayer->addChild(objCountLabel);
    this->m_vPageContent.push_back(objCountLabel);

    this->updateLayerItem(index, LayerManager::get()->getLayer(index));
}

void LayerViewPopup::updatePage() {
    for (auto const& node : this->m_vPageContent)
        node->removeFromParentAndCleanup(true);
    
    this->m_vPageContent.clear();

    for (int i = 0; i < page_max_item_count; i++) {
        if (i + page_max_item_count * this->m_nCurrentPage > LayerManager::max_layer_num)
            continue;

        float w = this->m_pLrSize.width - 150.0f, h = this->m_pLrSize.height - 180.0f;
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
    auto ui = LevelEditorLayer::get()->getEditorUI();

    ui->m_pCurrentLayerLabel->setString(CCString::createWithFormat("%d",
        LevelEditorLayer::get()->m_nCurrentLayer
    )->getCString());

    updateEditorLayerInputText(ui);

    g_nPage = this->m_nCurrentPage;

    BrownAlertDelegate::onClose(pSender);
}

void LayerViewPopup::onLock(CCObject* pSender) {
    auto btn = as<CCMenuItemSpriteExtra*>(pSender);
    auto index = as<int>(btn->getUserData());
    auto layer = LayerManager::get()->getLayer(index);

    if (!layer)
        return;
    
    layer->m_bLocked = !layer->m_bLocked;

    this->updateLockSprite(btn, layer);
}

void LayerViewPopup::onLockAll(CCObject* pSender) {
    for (auto i = 0; i < LayerManager::max_layer_num; i++) {
        auto layer = LayerManager::get()->getLayer(i);

        if (layer) {
            layer->m_bLocked = as<CCNode*>(pSender)->getUserData();

            auto btn = as<CCMenuItemSpriteExtra*>(this->m_pButtonMenu->getChildByTag(lock_tag_offset + i));
            
            if (btn) this->updateLockSprite(btn, layer);
        }
    }
}

void LayerViewPopup::onShow(CCObject* pSender) {
    auto btn = as<CCMenuItemSpriteExtra*>(pSender);
    auto index = as<int>(btn->getUserData());
    auto layer = LayerManager::get()->getLayer(index);

    if (!layer)
        return;
    
    layer->m_bVisible = !layer->m_bVisible;

    this->updateShowSprite(btn, layer);
}

void LayerViewPopup::onShowAll(CCObject* pSender) {
    for (auto i = 0; i < LayerManager::max_layer_num; i++) {
        auto layer = LayerManager::get()->getLayer(i);

        if (layer) {
            layer->m_bVisible = as<CCNode*>(pSender)->getUserData();

            auto btn = as<CCMenuItemSpriteExtra*>(this->m_pButtonMenu->getChildByTag(view_tag_offset + i));
            
            if (btn) this->updateShowSprite(btn, layer);
        }
    }
}

void LayerViewPopup::onSettings(CCObject* pSender) {
    auto layer = LayerManager::get()->getLayer(
        as<int>(as<CCMenuItemSpriteExtra*>(pSender)->getUserData())
    );

    if (!layer)
        return;

    LayerEditPopup::create(this, layer)->show();
}

void LayerViewPopup::onGoToLayer(CCObject* pSender) {
    auto btn = as<CCMenuItemSpriteExtra*>(pSender);
    auto index = as<int>(btn->getUserData());

    LevelEditorLayer::get()->setCurrentLayer(index);
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
