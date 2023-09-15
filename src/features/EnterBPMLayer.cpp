#include "EnterBPMLayer.hpp"

EnterBPMLayer* EnterBPMLayer::create() {
    auto pRet = new EnterBPMLayer();

    if (pRet && pRet->init(240.0f, 200.0f)) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}

EnterBPMLayer* EnterBPMLayer::setTarget(CreateGuidelinesLayer* t) {
    m_target = t;
    return this;
}

bool EnterBPMLayer::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->setTitle("Enter BPM");

    m_BPMInput = CCTextInputNode::create(120.0f, 60.0f, "BPM", "bigFont.fnt");
    m_BPMInput->setAllowedChars("0123456789.");
    m_BPMInput->setPosition(winSize.width / 2, winSize.height / 2 + 40.0f);
    this->m_mainLayer->addChild(m_BPMInput);

    m_offsetInput = CCTextInputNode::create(120.0f, 60.0f, "Offset", "bigFont.fnt");
    m_offsetInput->setAllowedChars("0123456789.");
    m_offsetInput->setPosition(winSize.width / 2, winSize.height / 2);
    this->m_mainLayer->addChild(m_offsetInput);

    m_patternInput = CCTextInputNode::create(120.0f, 60.0f, "Pattern", "bigFont.fnt");
    m_patternInput->setAllowedChars("gyoxGYOX ");
    m_patternInput->setPosition(winSize.width / 2, winSize.height / 2 - 40.0f);
    this->m_mainLayer->addChild(m_patternInput);

    auto createSprite = ButtonSprite::create("Create", 0, 0, "goldFont.fnt", "GJ_button_01.png", 0, .8f);
    createSprite->setScale(.8f);

    auto createBtn = CCMenuItemSpriteExtra::create(createSprite, this, menu_selector(EnterBPMLayer::onCreate));
    createBtn->setPosition(0.0f, - this->m_size.height / 2 + 25.0f);

    this->m_buttonMenu->addChild(createBtn);
    return true;
}

const char* patternCharToString(char c) {
    switch (c) {
        case 'O': case 'o': return "0.8";
        case 'Y': case 'y': return "0.9";
        case 'G': case 'g': return "1.0";
        default: return "0";
    }
}

void EnterBPMLayer::onCreate(CCObject* obj) {
    if (!m_target) return;

    if (!(m_BPMInput->getString() && strlen(m_BPMInput->getString())))
        return;

    float bpm = std::stof(m_BPMInput->getString());
    float offset = 0.0f;
    
    if (m_offsetInput->getString() && strlen(m_offsetInput->getString()))
        try {
            std::stof(m_offsetInput->getString());
        } catch (...) {}

    std::vector<char> pattern {};
    if (m_patternInput->getString() && strlen(m_patternInput->getString())) {
        for (auto ix = 0u; ix < strlen(m_patternInput->getString()); ix++) {
            auto c = m_patternInput->getString()[ix];

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
        guidelineString += std::to_string(secs) + "~" +
            patternCharToString(pattern.at(beat % pattern.size())) + "~";

        secs += secondsPerBeat;
        beat++;
    }

    m_target->m_guidelineString = guidelineString.data();
    m_target->onStop(nullptr);

    this->onClose(nullptr);
}

class CreateGuidelinesLayer_CB : public CreateGuidelinesLayer {
    public:
        void onEnterBPM(CCObject*) {
            EnterBPMLayer::create()->setTarget(this)->show();
        }
};
/*
class $modify(CreateGuidelinesLayer) {
    CreateGuidelinesLayer* create(LevelSettingsObject* lel) {
        auto cgl = CreateGuidelinesLayer::create(lel);

        auto BPMsprite = ButtonSprite::create("BPM", "goldFont.fnt", "GJ_button_04.png");
        BPMsprite->setScale(.8f);

        auto BPMbutton = CCMenuItemSpriteExtra::create(BPMsprite, this, menu_selector(CreateGuidelinesLayer_CB::onEnterBPM));
        BPMbutton->setPosition(120.0f, -50.0f);
        cgl->m_buttonMenu->addChild(BPMbutton);

        return cgl;
    }
};*/