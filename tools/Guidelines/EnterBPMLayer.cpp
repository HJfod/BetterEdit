#include "EnterBPMLayer.hpp"

std::string* std_string_operator_assign(std::string* str1, char* str2) {
    return as<std::string*(__thiscall*)(std::string*, char*)>(
        base + 0xf680
    )(str1, str2);
}

EnterBPMLayer* EnterBPMLayer::setTarget(CreateGuidelinesLayer* t) {
    m_pTarget = t;
    return this;
}

void EnterBPMLayer::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->m_pLayer->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(120.0f, "BPM", "0123456789."_s))
            .move(winSize.width / 2, winSize.height / 2 + 40.0f)
            .save(&m_pBPMInput)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(120.0f, "Offset", "0123456789."_s))
            .move(winSize.width / 2, winSize.height / 2)
            .save(&m_pOffsetInput)
            .done()
    );
    this->m_pLayer->addChild(
        CCNodeConstructor<InputNode*>()
            .fromNode(InputNode::create(120.0f, "Pattern", "gyoxGYOX "_s))
            .move(winSize.width / 2, winSize.height / 2 - 40.0f)
            .save(&m_pPatternInput)
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
                (SEL_MenuHandler)&EnterBPMLayer::onCreate
            ))
            .move(0.0f, - this->m_pLrSize.height / 2 + 25.0f)
            .done()
    );
}

const char* patternCharToString(char c) {
    switch (c) {
        case 'O': case 'o': return "0.8";
        case 'Y': case 'y': return "0.9";
        case 'G': case 'g': return "1.0";
        default: return "0";
    }
}

void EnterBPMLayer::onCreate(CCObject*) {
    if (!m_pTarget) return;

    if (!(m_pBPMInput->getString() && strlen(m_pBPMInput->getString())))
        return;

    float bpm = std::stof(m_pBPMInput->getString());
    float offset = 0.0f;
    
    if (m_pOffsetInput->getString() && strlen(m_pOffsetInput->getString()))
        try {
            offset = std::stof(m_pOffsetInput->getString());
        } catch (...) {}

    std::vector<char> pattern {};
    if (m_pPatternInput->getString() && strlen(m_pPatternInput->getString())) {
        for (auto ix = 0u; ix < strlen(m_pPatternInput->getString()); ix++) {
            auto c = m_pPatternInput->getString()[ix];

            switch (c) {
                // if some mf uses character filter bypass
                case 'G': case 'O': case 'Y': case 'X':
                case 'g': case 'o': case 'y': case 'x':
                    pattern.push_back(c);
                    break;
            }
        }
    } else
        pattern = { 'Y', 'G', 'G', 'G', };

    std::string guidelineString = "";

    auto secondsPerBeat = fabsf(60 / bpm);
    auto secs = offset / 1000.0f;
    auto beat = 0u;

    while (secs < 150.0f) {
        guidelineString += BetterEdit::formatToString(secs) + "~" +
            patternCharToString(pattern.at(beat % pattern.size())) + "~";

        secs += secondsPerBeat;
        beat++;
    }

    std_string_operator_assign(
        &m_pTarget->m_sGuidelineString,
        guidelineString.data()
    );
    m_pTarget->onStop(nullptr);

    this->onClose(nullptr);
}

EnterBPMLayer* EnterBPMLayer::create() {
    auto pRet = new EnterBPMLayer();

    if (pRet && pRet->init(240.0f, 200.0f, "GJ_square01.png", "Enter BPM")) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
