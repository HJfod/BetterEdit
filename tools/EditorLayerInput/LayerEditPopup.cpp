#include "LayerEditPopup.hpp"

void LayerEditPopup::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->m_pLayer->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(120.0f, "Name", inputf_Default, 20))
            .move(winSize / 2)
            .save(&this->m_pNameInput)
            .exec([this](auto p) -> void {
                if (this->m_pELayer->m_sName.size())
                    p->setString(this->m_pELayer->m_sName.c_str());
            })
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(120.0f, "Opacity", inputf_Numeral, 20))
            .move(winSize / 2 + CCPoint { 0, -40.0f })
            .save(&this->m_pOpacityInput)
            .exec([this](auto p) -> void {
                p->setString(std::to_string(this->m_pELayer->m_nOpacity).c_str());
            })
            .done()
    );

    this->m_pButtonMenu->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor()
                    .fromFrameName("GJ_resetBtn_001.png")
                    .scale(.8f)
                    .done(),
                this,
                (SEL_MenuHandler)&LayerEditPopup::onReset
            ))
            .move(this->m_pLrSize / 2 - CCPoint { 25.0f, 25.0f })
            .done()
    );
}

void LayerEditPopup::onReset(CCObject* pSender) {
    this->m_pNameInput->setString("");
    this->m_pOpacityInput->setString(std::to_string(LayerManager::default_opacity).c_str());
}

void LayerEditPopup::onClose(CCObject* pSender) {
    if (this->m_pNameInput->getString() && strlen(this->m_pNameInput->getString()))
        this->m_pELayer->m_sName = this->m_pNameInput->getString();
    else
        this->m_pELayer->m_sName = "";

    if (this->m_pOpacityInput->getString() && strlen(this->m_pOpacityInput->getString()))
        this->m_pELayer->m_nOpacity = std::stoi(this->m_pOpacityInput->getString());
    else
        this->m_pELayer->m_nOpacity = LayerManager::default_opacity;

    this->m_pViewPopup->updateLayerItem(this->m_pELayer->m_nLayerNumber, this->m_pELayer);

    BrownAlertDelegate::onClose(pSender);
}

LayerEditPopup* LayerEditPopup::create(LayerViewPopup* popup, LayerManager::Layer* layer) {
    auto pRet = new LayerEditPopup();

    if (pRet && (pRet->m_pELayer = layer) && (pRet->m_pViewPopup = popup) && pRet->init(
        250.0f, 180.0f, "GJ_square04.png",
        ("Layer " + std::to_string(layer->m_nLayerNumber)).c_str()
    )) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
