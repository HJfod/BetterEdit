#include "ColorTriggerPopup.hpp"

using namespace gd;
using namespace gdmake;
using namespace gdmake::extra;
using namespace cocos2d;

void ColorTriggerPopup::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->m_pLayer->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(60.0f, "0", "0123456789"_s))
            .move(winSize.width / 2 - 60.0f, winSize.height / 2)
            .save(&m_pRangeStartInput)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(60.0f, "0", "0123456789"_s))
            .move(winSize.width / 2 + 60.0f, winSize.height / 2)
            .save(&m_pRangeEndInput)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<CCLabelBMFont*>()
            .fromText("-", "bigFont.fnt")
            .move(winSize / 2)
            .scale(.7f)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<CCLabelBMFont*>()
            .fromText("Start ID:", "goldFont.fnt")
            .move(winSize.width / 2 - 60.0f, winSize.height / 2 + 30.0f)
            .scale(.6f)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<CCLabelBMFont*>()
            .fromText("End ID:", "goldFont.fnt")
            .move(winSize.width / 2 + 60.0f, winSize.height / 2 + 30.0f)
            .scale(.6f)
            .done()
    );

    this->m_pButtonMenu->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor()
                    .fromFrameName("GJ_infoIcon_001.png")
                    .scale(.8f)
                    .done(),
                this,
                (SEL_MenuHandler)&ColorTriggerPopup::onInfo
            ))
            .move(CCPoint { 0, 0 } - this->m_pLrSize / 2 + CCPoint { 25.0f, 25.0f })
            .done()
    );

    this->m_pButtonMenu->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<ButtonSprite*>()
                    .fromNode(ButtonSprite::create(
                        "Create", 0, 0, "goldFont.fnt", "GJ_button_01.png", 0, .8f
                    ))
                    .scale(.8f)
                    .done(),
                this,
                (SEL_MenuHandler)&ColorTriggerPopup::onCreate
            ))
            .move(0.0f, - this->m_pLrSize.height / 2 + 25.0f)
            .done()
    );
}

void ColorTriggerPopup::onCreate(CCObject* pSender) {
    // logic code here...

    int rangeStart = 0;
    int rangeEnd = 1012;

    if (this->m_pRangeStartInput &&
        this->m_pRangeStartInput->getString() &&
        strlen(this->m_pRangeStartInput->getString()))
        rangeStart = std::atoi(this->m_pRangeStartInput->getString());

    if (this->m_pRangeEndInput &&
        this->m_pRangeEndInput->getString() &&
        strlen(this->m_pRangeEndInput->getString()))
        rangeEnd = std::atoi(this->m_pRangeEndInput->getString());
    
    auto lel = GameManager::sharedState()->getEditorLayer();

    auto dict = lel->m_pLevelSettings->m_effectManager->m_colorActionDict;
    CCDictElement* el;
    auto pos = lel->getObjectLayer()->convertToNodeSpace(CCDirector::sharedDirector()->getWinSize() / 2);
    float y = pos.y;
    std::stringstream ss;

    CCDICT_FOREACH(dict, el) {
        const auto colorID = el->getIntKey();
        if (colorID < rangeStart || colorID > rangeEnd) continue;

        auto color = reinterpret_cast<gd::ColorAction*>(el->getObject());

        ss << "1,899,2," << pos.x << ",3," << y << ",10,0,36,1,23," << colorID << ",7," <<
            (int)(color->m_color.r) << ",8," << (int)(color->m_color.g) << ",9," <<
            (int)(color->m_color.b) << ",17," << color->m_blending << ",15," <<
            (color->m_playerColor == 1) << ",16," <<  (color->m_playerColor == 2) <<
            ",50," << color->m_copyID << ",49," << std::floor(color->m_copyHue) << "a"
            << color->m_copySaturation << "a" << color->m_copyBrightness << "a"
            << color->m_saturationChecked << "a" << color->m_brightnessChecked << ",35,"
            << color->m_opacity << ",60," << color->m_copyOpacity << ";";
        
        y += 30;
    }
    const auto result = ss.str();

    lel->getEditorUI()->pasteObjects(result);
    lel->getEditorUI()->updateButtons();

    if (this->m_pPauseLayer)
        this->m_pPauseLayer->onResume(pSender);

    this->onClose(pSender);
}

void ColorTriggerPopup::onInfo(CCObject*) {
    FLAlertLayer::create(
        nullptr,
        "Info",
        "OK", nullptr,
        300.0f,
        "Automatically creates <cy>color</c> triggers for each color "
        "you have set. By default it will create for <cl>all</c> modified "
        "channels, but you can also select a specific range.\n\n"
        "<co>Special colors</c> (<cr>BG</c>, <cr>3DL</c>, <cr>Obj</c> etc.) "
        "are IDs <cp>1000</c>-<cp>1012</c>."
    )->show();
}

ColorTriggerPopup* ColorTriggerPopup::create(EditorPauseLayer* pl) {
    auto pRet = new ColorTriggerPopup();

    if (pRet && pRet->init(250.0f, 170.0f, "GJ_square01.png", "Create Color Triggers")) {
        pRet->m_pPauseLayer = pl;
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}

