#include "LayerSettingsPopup.hpp"

void LayerSettingsPopup::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->m_pLayer->addChild(
        CCNodeConstructor<CCLabelBMFont*>()
            .fromText("Default Opacity:", "bigFont.fnt")
            .move(winSize / 2 + CCPoint { 0.0f, 20.0f })
            .scale(.5f)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<Slider*>()
            .fromNode(Slider::create(this, (SEL_MenuHandler)&LayerSettingsPopup::sliderChanged, .8f))
            .move(winSize / 2)
            .save(&this->m_pOpacitySlider)
            .exec([this](Slider* p) -> void {
                p->setValue(LayerManager::get()->default_opacity / 255.0f);
                
                p->updateBar();
            })
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<CCLabelBMFont*>()
            .fromText("", "goldFont.fnt")
            .move(winSize / 2 + CCPoint { 100.0f, 0.0f })
            .scale(.5f)
            .save(&this->m_pOpacityLabel)
            .exec([this](CCLabelBMFont* p) -> void {})
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
                (SEL_MenuHandler)&LayerSettingsPopup::onReset
            ))
            .move(this->m_pLrSize / 2 - CCPoint { 25.0f, 25.0f })
            .done()
    );

    this->sliderChanged(nullptr);
}

void LayerSettingsPopup::sliderChanged(CCObject*) {
    this->m_pOpacityLabel->setString(std::to_string(
        static_cast<int>(this->m_pOpacitySlider->getValue() * 255)
    ).c_str());
}

void LayerSettingsPopup::onReset(CCObject* pSender) {
    this->m_pOpacitySlider->setValue(LayerManager::get()->default_gd_opacity / 255.0f);
    this->m_pOpacitySlider->updateBar();
    this->sliderChanged(nullptr);
}

void LayerSettingsPopup::onClose(CCObject* pSender) {
    LayerManager::get()->default_opacity = static_cast<int>(m_pOpacitySlider->getValue() * 255);

    BrownAlertDelegate::onClose(pSender);
}

LayerSettingsPopup* LayerSettingsPopup::create(LayerViewPopup* popup) {
    auto pRet = new LayerSettingsPopup();

    if (pRet &&  (pRet->m_pViewPopup = popup) && pRet->init(
        250.0f, 180.0f, "GJ_square02.png", "Layer Settings"
    )) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
