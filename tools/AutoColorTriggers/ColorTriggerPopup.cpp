#include "ColorTriggerPopup.hpp"
#include <MonoSpaceLabel.hpp>

using namespace gd;
using namespace gdmake;
using namespace gdmake::extra;
using namespace cocos2d;

void ColorTriggerPopup::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

//     auto label = MonoSpaceLabel::create("", "chatFont.fnt", 10.0f, 350.0f);

//     label->setPosition(winSize / 2);
//     label->setScale(.8f);
//     label->setString(
// R"CPP(
// auto label = MonoSpaceLabel::create("", "chatFont.fnt", 10.0f, 350.0f);
// label->setPosition(winSize / 2);
// label->setPosition(winSize / 2);
// )CPP");

//     this->m_pLayer->addChild(label);

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

    auto dict = lel->m_pLevelSettings->m_pEffectManager->m_colorActionDict;
    CCDictElement* el;
    auto pos = lel->getObjectLayer()->convertToNodeSpace(CCDirector::sharedDirector()->getWinSize() / 2);
    float y = pos.y;
    std::stringstream ss;

    lel->getEditorUI()->deselectAll();

    auto objs = CCArray::create();
    CCDICT_FOREACH(dict, el) {
        const auto colorID = el->getIntKey();
        if (colorID < rangeStart || colorID > rangeEnd) continue;

        auto color = as<ColorAction*>(el->getObject());

        auto obj = as<EffectGameObject*>(lel->createObject(899, { pos.x, y }, false));

        obj->m_nTargetColorID = colorID;
        obj->m_colColor = color->m_color;
        obj->m_bBlending = color->m_blending;
        obj->m_fOpacity = color->m_opacity;
        obj->m_bPlayerColor1 = color->m_playerColor == 1;
        obj->m_bPlayerColor2 = color->m_playerColor == 2;
        obj->m_nCopyColorID = color->m_copyID;
        obj->m_obHSVValue = color->m_obCopyHSV;
        obj->updateLabel();

        if (lel->m_nCurrentLayer != -1)
            obj->m_nEditorLayer = lel->m_nCurrentLayer;

        objs->addObject(obj);
        
        y += 30.0f;
    }

    lel->getEditorUI()->selectObjects(objs, true);
    objs->release();

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

