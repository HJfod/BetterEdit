#include "IDRemapPopup.hpp"

#define GDML(gdml)

void IDRemapPopup::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->m_pLayer->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(60.0f, "0", "0123456789"_s))
            .move(winSize.width / 2 - 60.0f, winSize.height / 2 + 30.0f)
            .save(&m_pStartInput)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(60.0f, "0", "0123456789"_s))
            .move(winSize.width / 2 + 60.0f, winSize.height / 2 + 30.0f)
            .save(&m_pEndInput)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<CCLabelBMFont*>()
            .fromText("Start ID:", "goldFont.fnt")
            .move(winSize.width / 2 - 60.0f, winSize.height / 2 + 60.0f)
            .scale(.6f)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<CCLabelBMFont*>()
            .fromText("End ID:", "goldFont.fnt")
            .move(winSize.width / 2 + 60.0f, winSize.height / 2 + 60.0f)
            .scale(.6f)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(60.0f, "0", "0123456789-"_s))
            .move(winSize.width / 2, winSize.height / 2 - 30.0f)
            .save(&m_pOffsetInput)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<CCLabelBMFont*>()
            .fromText("Offset:", "goldFont.fnt")
            .move(winSize.width / 2, winSize.height / 2)
            .scale(.6f)
            .done()
    );

    this->m_pButtonMenu->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor<ButtonSprite*>()
                    .fromNode(ButtonSprite::create(
                        "Remap", 0, 0, "goldFont.fnt", "GJ_button_01.png", 0, .8f
                    ))
                    .scale(.8f)
                    .done(),
                this,
                (SEL_MenuHandler)&IDRemapPopup::onRemap
            ))
            .move(0.0f, - this->m_pLrSize.height / 2 + 25.0f)
            .done()
    );
}

void IDRemapPopup::onRemap(CCObject* pSender) {
    int start, end, offset;

    if (!(this->m_pStartInput &&
        this->m_pStartInput->getString() &&
        strlen(this->m_pStartInput->getString())))
        return;

    if (!(this->m_pEndInput &&
        this->m_pEndInput->getString() &&
        strlen(this->m_pEndInput->getString())))
        return;

    if (!(this->m_pOffsetInput &&
        this->m_pOffsetInput->getString() &&
        strlen(this->m_pOffsetInput->getString())))
        return;

    try {
        start = std::stoi(this->m_pStartInput->getString());
        end = std::stoi(this->m_pEndInput->getString());
        offset = std::stoi(this->m_pOffsetInput->getString());
    } catch (...) {
        return;
    }

    auto editCount = 0u;
    auto remapCount = 0u;
    CCARRAY_FOREACH_B_TYPE(LevelEditorLayer::get()->getAllObjects(), obj, GameObject) {
        bool edited = false;

        for (auto i = 0; i < obj->m_nGroupCount; i++)
            if (start <= obj->m_pGroups[i] && obj->m_pGroups[i] <= end) {
                obj->m_pGroups[i] += offset;
                edited = true;
                remapCount++;
            }
        
        if (edited)
            editCount++;
    }

    this->onClose(nullptr);

    FLAlertLayer::create(
        nullptr,
        "Remap Done",
        "OK", nullptr,
        "<co>" + std::to_string(editCount) + "</c> objects edited\n"
        "<cc>" + std::to_string(remapCount) + "</c> groups remapped"
    )->show();
}

IDRemapPopup* IDRemapPopup::create() {
    auto pRet = new IDRemapPopup();

    if (pRet && pRet->init(300.0f, 240.0f, "GJ_square01.png", "Remap IDs")) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
