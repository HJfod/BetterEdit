#include "LayerEditNode.hpp"

CCSprite* updateBtnSprite(CCMenuItemSpriteExtra* btn, CCSprite* nspr) {
    auto spr = btn->getNormalImage();

    if (btn->getTag() == 0) {
        auto cspr = as<CCSprite*>(spr->getChildByTag(1));

        if (cspr) {
            cspr->retain();
            spr->removeChildByTag(1);
            
            nspr->setScale(cspr->getScale());
            nspr->setContentSize(cspr->getContentSize());
            nspr->setPosition(cspr->getPosition());
            nspr->setAnchorPoint(cspr->getAnchorPoint());
            nspr->setTag(cspr->getTag());
            nspr->setZOrder(cspr->getZOrder());

            spr->addChild(nspr);

            cspr->release();

            return nspr;
        }
    }

    spr->retain();

    nspr->setScale(spr->getScale());
    nspr->setContentSize(spr->getContentSize());

    btn->setNormalImage(nspr);

    nspr->setPosition(spr->getPosition());
    nspr->setAnchorPoint(spr->getAnchorPoint());
    nspr->setScale(spr->getScale());

    spr->release();

    return nspr;
}

void updateLockSprite(CCMenuItemSpriteExtra* btn, LayerManager::Layer* layer) {
    auto spr = updateBtnSprite(btn, CCSprite::createWithSpriteFrameName(
        layer && layer->m_bLocked ? 
        "GJ_lock_001.png" :
        "GJ_lock_open_001.png")
    );
    
    spr->setOpacity(layer && layer ? 255 : 120);

    if (layer && !layer->m_bLocked)
        spr->setAnchorPoint({ 0.48f, 0.48f });
    else
        spr->setAnchorPoint({ 0.5f, 0.5f });
}

void updateShowSprite(CCMenuItemSpriteExtra* btn, LayerManager::Layer* layer) {
    const char* fname = "BE_eye-off.png";

    if (layer && layer->m_bVisible) {
        if (layer->m_nOpacity != LayerManager::use_default_opacity)
            fname = "BE_eye-on-alt.png";
        else
            fname = "BE_eye-on.png";
    }

    updateBtnSprite(btn, CCSprite::create(fname));
}


void LayerEditNode::updateNode(LayerManager::Layer* layer) {
    this->m_pELayer = layer;

    if (!layer) return;

    if (this->m_pELayer->m_sName.size())
        this->m_pNameInput->setString(this->m_pELayer->m_sName.c_str());

    if (this->m_pELayer->m_nOpacity == LayerManager::use_default_opacity)
        this->m_pOpacitySlider->setValue(LayerManager::get()->default_opacity / 255.0f);
    else
        this->m_pOpacitySlider->setValue(this->m_pELayer->m_nOpacity / 255.0f);
    
    this->m_pOpacitySlider->updateBar();

    this->sliderChanged(nullptr);
    updateLockSprite(this->m_pLockBtn, this->m_pELayer);
    updateShowSprite(this->m_pShowBtn, this->m_pELayer);
}

bool LayerEditNode::init(LayerViewPopup* popup, LayerManager::Layer* layer) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->m_pELayer = layer;
    this->m_pViewPopup = popup;

    this->addChild(
        CCNodeConstructor<CCLabelBMFont*>()
            .fromText("Name:", "bigFont.fnt")
            .move(0, 52.0f)
            .scale(.5f)
            .done()
    );
    this->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(120.0f, "Name", inputf_Default, 20))
            .move(0, 25.0f)
            .save(&this->m_pNameInput)
            .tag(s_nameInputTag)
            .done()
    );
    this->addChild(
        CCNodeConstructor<CCLabelBMFont*>()
            .fromText("Opacity:", "bigFont.fnt")
            .move(-15.0f, -10.0f)
            .scale(.5f)
            .done()
    );
    this->addChild(
        CCNodeConstructor<Slider*>()
            .fromNode(Slider::create(this, (SEL_MenuHandler)&LayerEditNode::sliderChanged, .7f))
            .move(0, -30.0f)
            .save(&this->m_pOpacitySlider)
            .done()
    );
    this->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(60.0f, "", "bigFont.fnt", inputf_Numeral, 3))
            .move(40.0f, -10.0f)
            .scale(.5f)
            .save(&this->m_pOpacityLabel)
            .exec([this](InputNode* n) -> void {
                n->getInputNode()->setDelegate(this);
            })
            .tag(s_opacityInputTag)
            .done()
    );
    this->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor()
                    .fromFrameName("GJ_resetBtn_001.png")
                    .scale(.5f)
                    .done(),
                this,
                (SEL_MenuHandler)&LayerEditNode::onResetOpacity
            ))
            .move(70.0f, -10.0f)
            .save(&this->m_pResetOpacityBtn)
            .done()
    );
    this->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor()
                    .fromFrameName("GJ_lock_001.png")
                    .scale(.9f)
                    .exec([](auto t) -> void {
                        t->setContentSize(t->getScaledContentSize());
                    })
                    .done(),
                this,
                (SEL_MenuHandler)&LayerEditNode::onLock
            ))
            .move(-15.0f, -50.0f)
            .save(&this->m_pLockBtn)
            .done()
    );
    this->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor()
                    .fromFile("BE_eye-on.png")
                    .scale(.7f)
                    .exec([](auto t) -> void {
                        t->setContentSize(t->getScaledContentSize());
                    })
                    .done(),
                this,
                (SEL_MenuHandler)&LayerEditNode::onShow
            ))
            .move(15.0f, -50.0f)
            .save(&this->m_pShowBtn)
            .done()
    );
    this->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor()
                    .fromFrameName("GJ_resetBtn_001.png")
                    .scale(.8f)
                    .done(),
                this,
                (SEL_MenuHandler)&LayerEditNode::onReset
            ))
            .move(150.0f, 150.0f)
            .done()
    );

    this->registerWithTouchDispatcher();
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->incrementForcePrio(2);
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);

    this->updateNode(layer);

    this->sliderChanged(nullptr);
    updateLockSprite(this->m_pLockBtn, this->m_pELayer);
    updateShowSprite(this->m_pShowBtn, this->m_pELayer);
}

void LayerEditNode::onShow(CCObject* pSender) {
    auto btn = as<CCMenuItemSpriteExtra*>(pSender);
    
    if (!this->m_pELayer) return;

    this->m_pELayer->m_bVisible = !this->m_pELayer->m_bVisible;

    updateShowSprite(btn, this->m_pELayer);
}

void LayerEditNode::onLock(CCObject* pSender) {
    auto btn = as<CCMenuItemSpriteExtra*>(pSender);
    
    if (!this->m_pELayer) return;

    this->m_pELayer->m_bLocked = !this->m_pELayer->m_bLocked;

    updateLockSprite(btn, this->m_pELayer);
}

void LayerEditNode::sliderChanged(CCObject* pSender) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    if (!this->m_pELayer) return;

    auto o = static_cast<int>(this->m_pOpacitySlider->getValue() * 255);

    // for the mfs with slider limit bypass
    if (o < 0) o = 0;
    if (o > 255) o = 255;

    if (o == LayerManager::get()->default_opacity)
        this->m_pELayer->m_nOpacity = LayerManager::use_default_opacity;
    else
        this->m_pELayer->m_nOpacity = o;

    this->m_pOpacityLabel->setString(std::to_string(o).c_str());

    this->m_pResetOpacityBtn->setVisible(
        o != LayerManager::get()->default_opacity
    );
    
    updateShowSprite(this->m_pShowBtn, this->m_pELayer);
    this->m_pViewPopup->updateLayerItem(this->m_pELayer->m_nLayerNumber, this->m_pELayer);
}

void LayerEditNode::onResetOpacity(CCObject* pSender) {
    this->m_pOpacitySlider->setValue(LayerManager::get()->default_opacity / 255.0f);
    this->m_pOpacitySlider->updateBar();
    this->sliderChanged(nullptr);
}

void LayerEditNode::onReset(CCObject* pSender) {
    this->m_pNameInput->setString("");
    this->onResetOpacity(pSender);
}

void LayerEditNode::textChanged(CCTextInputNode* pInput) {
    switch (pInput->getTag()) {
        case s_opacityInputTag: {
            int o = LayerManager::get()->default_opacity;

            if (!this->m_pELayer) return;

            try { o = std::stoi(pInput->getString()); } catch (...) {}

            // for the mfs with mhv6 character filter & text length bypass
            if (o < 0) o = 0;
            if (o > 255) o = 255;

            if (o == LayerManager::get()->default_opacity)
                this->m_pELayer->m_nOpacity = LayerManager::use_default_opacity;
            else
                this->m_pELayer->m_nOpacity = o;

            this->m_pOpacitySlider->setValue(o / 255.0f);
            this->m_pOpacitySlider->updateBar();
            this->m_pResetOpacityBtn->setVisible(
                o != LayerManager::get()->default_opacity
            );
            updateShowSprite(this->m_pShowBtn, this->m_pELayer);
            this->m_pViewPopup->updateLayerItem(this->m_pELayer->m_nLayerNumber, this->m_pELayer);
        } break;

        case s_nameInputTag: {
            if (this->m_pNameInput->getString() && strlen(this->m_pNameInput->getString()))
                this->m_pELayer->m_sName = this->m_pNameInput->getString();
            else
                this->m_pELayer->m_sName = "";
        } break;
    }
}

LayerEditNode* LayerEditNode::create(LayerViewPopup* popup, LayerManager::Layer* layer) {
    auto pRet = new LayerEditNode();

    if (pRet && pRet->init(popup, layer)) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
