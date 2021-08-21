#include "../BetterEdit.hpp"
#include <InputPrompt.hpp>

std::string g_nextFreeColorInput = "";
static constexpr const int NEXTFREE_TAG = 5000;
static constexpr const int COLORBTN_LABEL_TAG = 999;
static constexpr const int COLORBTN_SELECTION_TAG = 1001;
static constexpr const int colorCountOnPage = 24;

class CustomizeObjectLayer_CB : public CustomizeObjectLayer {
    public:
        void onCustomNextFree(CCObject* pSender) {
            auto p = InputPrompt::create("Next Free Offset", "ID", [this, pSender](const char* txt) -> void {
                if (txt && strlen(txt)) {
                    auto startID = 0;
                    try { startID = std::stoi(txt); } catch (...) {}

                    auto bytes = intToBytes(startID);
                    bytes.insert(bytes.begin(), 0xbf);

                    patchBytes(0x164fa5, bytes);
                    this->onNextColorChannel(pSender);

                    g_nextFreeColorInput = txt;
                } else {
                    g_nextFreeColorInput = "";
                    patchBytes(0x164fa5, { 0xbf, 0x01, 0x00, 0x00, 0x00 });
                }

                auto btn = as<CCMenuItemSpriteExtra*>(this->m_pButtonMenu->getChildByTag(NEXTFREE_TAG));

                if (btn) {
                    auto spr = as<ButtonSprite*>(btn->getNormalImage());
                    spr->setString(g_nextFreeColorInput.size() ? g_nextFreeColorInput.c_str() : "0");
                    spr->updateBGImage(
                        g_nextFreeColorInput.size() ? "GJ_button_02.png" : "GJ_button_04.png"
                    );
                }
            }, "Apply")->setApplyOnEsc(true)->setTrashButton(true);
            p->getInputNode()->getInputNode()->setAllowedChars("0123456789");
            p->getInputNode()->getInputNode()->setMaxLabelLength(6);
            p->getInputNode()->setString(g_nextFreeColorInput.c_str());
            p->show();
        }
};

std::string getShortColorBtnText(int channel) {
    switch (channel) {
        case 0: return "D";
        case 1000: return "BG";
        case 1001: return "G1";
        case 1002: return "L";
        case 1003: return "3DL";
        case 1004: return "Obj";
        case 1005: return "P1";
        case 1006: return "P2";
        case 1007: return "LBG";
        case 1008: return "C";
        case 1009: return "G2";
        case 1010: return "";   // black
        case 1011: return "";   // white
        case 1012: return "Lighter";
        default: return std::to_string(channel);
    }
}

void updateCustomChannelSprite(CustomizeObjectLayer* self, ColorChannelSprite* btn) {
    std::string txt = GJSpecialColorSelect::textForColorIdx(self->m_nCustomColorChannel);
    
    if (txt == "NA")
        try {
            txt = std::to_string(self->m_nCustomColorChannel);
        } catch(...) {
            txt = std::to_string(colorCountOnPage);
        }
    
    auto label = as<CCLabelBMFont*>(
        btn->getChildByTag(COLORBTN_LABEL_TAG)
    );

    if (label) label->setString(txt.c_str());
}

void updateButtons(CustomizeObjectLayer* self) {
    if (BetterEdit::getDisableNewColorSelection())
        return;

    // wacky hack to make the input not call the delegate
    auto oldDelegate = self->m_pCustomColorInput->m_delegate;
    self->m_pCustomColorInput->setDelegate(nullptr);
    self->m_pCustomColorInput->setString(std::to_string(self->m_nCustomColorChannel).c_str());
    self->m_pCustomColorInput->setDelegate(oldDelegate);

    CCARRAY_FOREACH_B_TYPE(self->m_pColorButtons, btn, ColorChannelSprite) {
        int channel;

        if (!btn->getChildByTag(COLORBTN_LABEL_TAG))
            continue;

        if (btn->getParent()->getTag() == 1008)
            channel = self->m_nCustomColorChannel;
        else
            channel = btn->getParent()->getTag();
        
        self->m_bCustomColorSelected = btn->getParent()->getTag() == 1008 &&
            self->getActiveMode(true) == channel;

        auto col = LevelEditorLayer::get()->m_pLevelSettings->m_pEffectManager->getColorAction(channel);

        btn->updateValues(col);

        switch (channel) {
            case 1010:
                btn->setColor({ 0, 0, 0 });
                break;
            case 0:
                btn->setOpacity(50);
                break;
            case 1007:
                btn->setOpacity(120);
                break;
        }

        auto sSprite = reinterpret_cast<CCSprite*>(
            btn->getChildByTag(COLORBTN_SELECTION_TAG)
        );

        if (sSprite)
            sSprite->setVisible(self->getActiveMode(true) == channel);

        auto label = reinterpret_cast<CCLabelBMFont*>(
            btn->getChildByTag(COLORBTN_LABEL_TAG)
        );
        
        if (label) {
            label->setVisible(!col->m_copyID);

            if (btn->getParent()->getTag() == 1008)
                updateCustomChannelSprite(self, btn);
        }
    }

    self->m_pArrowDown->setVisible(self->m_bCustomColorSelected);
    self->m_pArrowUp->setVisible(self->m_bCustomColorSelected);
    self->m_pCustomColorInput->setVisible(self->m_bCustomColorSelected);
    self->m_pCustomColorInput->detachWithIME();
    self->m_pCustomColorInputBG->setVisible(self->m_bCustomColorSelected);
}

ColorChannelSprite* getChannelSprite(int channel) {
    auto col = LevelEditorLayer::get()->m_pLevelSettings->m_pEffectManager->getColorAction(channel);

    auto channelSprite = ColorChannelSprite::create();

    channelSprite->updateValues(col);
    channelSprite->setPosition({ 0, 0 });
    channelSprite->setScale(.8f);

    auto selSprite = CCSprite::createWithSpriteFrameName("GJ_select_001.png");

    selSprite->setScale(1.1f);
    selSprite->setTag(COLORBTN_SELECTION_TAG);
    selSprite->setPosition(channelSprite->getContentSize() / 2);
    selSprite->setVisible(false);

    channelSprite->addChild(selSprite);

    auto label = CCLabelBMFont::create(
        getShortColorBtnText(channel).c_str(),
        "bigFont.fnt"
    );

    label->limitLabelWidth(25.0f, .4f, .2f);
    label->setPosition(channelSprite->getContentSize() / 2);
    label->setTag(COLORBTN_LABEL_TAG);
    
    if (channel == 1008) {
        channelSprite->setOpacity(200);
    }

    channelSprite->addChild(label);

    return channelSprite;
}

void updateCustomChannelSprite(CustomizeObjectLayer* self) {
    if (BetterEdit::getDisableNewColorSelection())
        return;

    CCARRAY_FOREACH_B_TYPE(self->m_pColorButtons, btn, ColorChannelSprite) {
        auto label = as<CCLabelBMFont*>(
            btn->getChildByTag(COLORBTN_LABEL_TAG)
        );

        if (!label) continue;

        if (btn->getParent()->getTag() == 1008)
            updateCustomChannelSprite(self, btn);
    }
}

CCSprite* createBGSprite() {
    auto bgTexture = CCRenderTexture::create(365, 260);

    bgTexture->begin();

    auto bg = CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );

    bg->setContentSize({ 345.0f, 145.0f });
    bg->setPosition(
        182.5f, 130.0f
    );
    bg->setColor({ 0, 0, 0 });

    // auto bg2 = CCScale9Sprite::create(
    //     "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    // );

    // bg2->setContentSize({ 182.0f, 145.0f });
    // bg2->setPosition(
    //     223.5f, 160.0f
    // );
    // bg2->setColor({ 0, 0, 0 });

    bg->visit();
    // bg2->visit();

    bgTexture->end();

    return bgTexture->getSprite();
}

GDMAKE_HOOK(0x56db0)
void __fastcall CustomizeObjectLayer_onSelectMode(
    CustomizeObjectLayer* self,
    edx_t edx,
    CCObject* pSender
) {
    GDMAKE_ORIG_V(self, edx, pSender);

    updateButtons(self);
}

GDMAKE_HOOK(0x53e00)
bool __fastcall CustomizeObjectLayer_init(
    CustomizeObjectLayer* self,
    edx_t edx,
    EffectGameObject* obj,
    CCArray* objs
) {
    if (BetterEdit::getDisableNewColorSelection())
        unpatch(0x5737e);
    else
        patch(0x5737e, { 0xb9, colorCountOnPage });

    if (!GDMAKE_ORIG(self, edx, obj, objs))
        return false;

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    
    if (
        !BetterEdit::getDisableNewColorSelection() &&
        self->m_nCustomColorChannel < colorCountOnPage
    )
        self->m_nCustomColorChannel = colorCountOnPage;

    auto nextFreeBtn = getChild<CCMenuItemSpriteExtra*>(self->m_pButtonMenu, 26);

    nextFreeBtn->setPositionX(nextFreeBtn->getPositionX() - 2.0f);

    auto customNextFreeBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create(
            g_nextFreeColorInput.size() ? g_nextFreeColorInput.c_str() : "0", 20, true, "goldFont.fnt", 
            g_nextFreeColorInput.size() ? "GJ_button_02.png" : "GJ_button_04.png", 25, .6f
        ),
        self,
        (SEL_MenuHandler)&CustomizeObjectLayer_CB::onCustomNextFree
    );
    customNextFreeBtn->setTag(NEXTFREE_TAG);
    customNextFreeBtn->setPosition(
        nextFreeBtn->getPositionX() + 58.0f,
        nextFreeBtn->getPositionY()
    );
    self->m_pColorNodes->addObject(customNextFreeBtn);
    self->m_pButtonMenu->addChild(customNextFreeBtn);

    if (BetterEdit::getDisableNewColorSelection())
        return true;
    
    self->m_pTitleLabel->setVisible(false);

    getChild<CCLabelBMFont*>(self->m_pButtonMenu, 1)->setPosition(
        210, 0
    );

    CCARRAY_FOREACH_B_TYPE(self->m_pColorButtons, btn, ButtonSprite)
        btn->getParent()->removeFromParent();
    self->m_pColorButtons->removeAllObjects();

    auto bgSprite = createBGSprite();

    bgSprite->setOpacity(100);
    bgSprite->setPosition(winSize / 2);

    self->m_pColorNodes->addObject(bgSprite);
    self->m_pLayer->addChild(bgSprite);

    int i = 0;
    for (auto channel : std::initializer_list<int> {
        0, 1000, 1001, 1009,
        1005, 1003, 1004, 1002,
        1006, 1007, 1010, 1011,
    }) {
        auto channelSprite = getChannelSprite(channel);

        auto btn = CCMenuItemSpriteExtra::create(
            channelSprite, self, (SEL_MenuHandler)&CustomizeObjectLayer::onSelectColor
        );

        self->m_pColorButtons->addObject(channelSprite);
        self->m_pColorNodes->addObject(btn);
        self->m_pButtonMenu->addChild(btn);

        auto width = 100.0f;
        auto yoff = 35.0f;
        constexpr const int row_c = 4;

        btn->setPosition(
            static_cast<int>(floor(i / row_c)) * yoff - 150.0f,
            (1.0f - ((i % row_c) / 3.0f)) * width + 45.0f
        );
        btn->setTag(channel);

        i++;
    }

    for (int i = 0; i < colorCountOnPage; i++) {
        auto channel = i + 1;
        if (i == colorCountOnPage - 1)
            channel = 1008;

        auto channelSprite = getChannelSprite(channel);

        auto btn = CCMenuItemSpriteExtra::create(
            channelSprite, self, (SEL_MenuHandler)&CustomizeObjectLayer::onSelectColor
        );

        self->m_pColorButtons->addObject(channelSprite);
        self->m_pColorNodes->addObject(btn);
        self->m_pButtonMenu->addChild(btn);

        auto width = 180.0f;
        auto yoff = 33.5f;
        constexpr const int row_c = 6;

        btn->setPosition(
            ((i % row_c) / static_cast<float>(row_c - 1.0f)) * width - 30.0f,
            static_cast<int>(floor(i / row_c)) * -yoff + 145.0f
        );
        btn->setTag(channel);
    }

    // for (int i = 0; i < 5; i++) {
    //     auto label = CCLabelBMFont::create(std::to_string(i + 1).c_str(), "bigFont.fnt");

    //     label->setScale(.6f);

    //     auto btn = CCMenuItemSpriteExtra::create(
    //         label, self, nullptr
    //     );

    //     auto width = 150.0f;
    //     btn->setPosition(
    //         (i / 4.0f) * width + 250.0f,
    //         225.0f
    //     );

    //     self->m_pColorNodes->addObject(btn);
    //     self->addChild(btn);
    // }

    auto line = CCSprite::createWithSpriteFrameName("edit_vLine_001.png");

    line->setPosition({ winSize.width / 2 - 55.0f, winSize.height / 2 });
    line->setScaleY(1.65f);
    line->setOpacity(100);

    self->m_pColorNodes->addObject(line);
    self->m_pLayer->addChild(line);

    updateButtons(self);

    auto showInput = self->getActiveMode(true) == self->m_nCustomColorChannel;
    self->m_pArrowDown->setVisible(showInput);
    self->m_pArrowUp->setVisible(showInput);
    self->m_pCustomColorInput->setVisible(showInput);
    self->m_pCustomColorInput->detachWithIME();
    self->m_pCustomColorInputBG->setVisible(showInput);

    return true;
}

GDMAKE_HOOK(0x574d0)
void __fastcall CustomizeObjectLayer_textChanged(
    CustomizeObjectLayer* self_, edx_t edx, CCTextInputNode* input
) {
    GDMAKE_ORIG_V(self_, edx, input);

    auto self = as<CustomizeObjectLayer*>(as<uintptr_t>(self_) - 0x1cc);

    if (input->getString() && strlen(input->getString()))
        try {
            self->m_nCustomColorChannel = std::stoi(input->getString());
        } catch(...) {}
    
    if (self->m_nCustomColorChannel < 1)
        self->m_nCustomColorChannel = 1;

    updateCustomChannelSprite(self);
}

GDMAKE_HOOK(0x57350)
void __fastcall CustomizeObjectLayer_onUpdateCustomColor(
    CustomizeObjectLayer* self,
    edx_t edx,
    CCObject* pSender
) {
    GDMAKE_ORIG(self, edx, pSender);

    updateButtons(self);
}

GDMAKE_HOOK(0x564a0)
void __fastcall CustomizeObjectLayer_colorSelectClosed(
    CustomizeObjectLayer* self
) {
    updateButtons(as<CustomizeObjectLayer*>(as<uintptr_t>(self) - 0x1d4));

    GDMAKE_ORIG(self);
}

GDMAKE_HOOK(0x579d0)
void __fastcall CustomizeObjectLayer_highlightSelected(
    CustomizeObjectLayer* self,
    edx_t edx,
    ButtonSprite* bspr
) {
    if (BetterEdit::getDisableNewColorSelection())
        return GDMAKE_ORIG_V(self, edx, bspr);

    if (self->m_nCustomColorChannel < colorCountOnPage)
        self->m_nCustomColorChannel = colorCountOnPage;

    updateButtons(self);
}
