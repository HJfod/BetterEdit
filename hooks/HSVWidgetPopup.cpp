#include <GDMake.h>
#include <GUI/CCControlExtension/CCScale9Sprite.h>

using namespace gdmake;
using namespace gdmake::extra;
using namespace gd;
using namespace cocos2d;

int g_hsvColorChannel = 0;
static constexpr const int HSVSQUARE1_TAG = 0xB00B5; // i am incredibly mature
static constexpr const int HSVSQUARE2_TAG = 0xB00BA;

class AddHSVTextDelegate : public CCNode, public TextInputDelegate {
    public:
        ConfigureHSVWidget* m_pGIL;

        virtual void textChanged(CCTextInputNode* input) override {
            float val = 0.0f;

            if (input->getString() && strlen(input->getString()))
                val = static_cast<float>(std::atof(input->getString()));

            switch (reinterpret_cast<int>(input->getUserData())) {
                case 5:
                    m_pGIL->m_pHueSlider->setValue((val + 180.0f) / 360.0f);
                    m_pGIL->m_obValue.h = val;
                    break;

                case 6:
                    if (m_pGIL->m_obValue.absoluteSaturation)
                        m_pGIL->m_pSaturationSlider->setValue((val + 1.0f) / 2);
                    else
                        m_pGIL->m_pSaturationSlider->setValue(val / 2);
                    m_pGIL->m_obValue.s = val;
                    break;

                case 7:
                    if (m_pGIL->m_obValue.absoluteBrightness)
                        m_pGIL->m_pBrightnessSlider->setValue((val + 1.0f) / 2);
                    else
                        m_pGIL->m_pBrightnessSlider->setValue(val / 2);
                    m_pGIL->m_obValue.v = val;
                    break;
            }
        }

        static AddHSVTextDelegate* create(ConfigureHSVWidget* gil) {
            auto ret = new AddHSVTextDelegate();

            if (ret && ret->init()) {
                ret->m_pGIL = gil;
                ret->autorelease();
                return ret;
            }

            CC_SAFE_DELETE(ret);
            return nullptr;
        }
};

void updateInputText(CCTextInputNode* upd, float val) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << val;
    std::string s = stream.str();

    upd->setString(s.c_str());
}

void turnLabelIntoInput(
    ConfigureHSVWidget* self,
    CCLabelBMFont* text,
    AddHSVTextDelegate* ad,
    int id,
    float ov,
    const char* title,
    std::string const& ac = "-0123456789."
) {
    text->setVisible(false);

    auto spr = extension::CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );

    spr->setScale(.3f);
    spr->setColor({ 0, 0, 0 });
    spr->setOpacity(100);
    spr->setContentSize({ 235.0f, 70.0f });
    spr->setPosition(text->getPosition() + CCPoint { 50.0f, 0.0f });

    auto eLayerInput = CCTextInputNode::create("0", ad, "bigFont.fnt", 70.0f, 20.0f);

    eLayerInput->setPosition(text->getPosition() + CCPoint { 50.0f, 0.0f });
    eLayerInput->setLabelPlaceholderColor({ 120, 120, 120 });
    eLayerInput->setAllowedChars(ac);
    eLayerInput->setAnchorPoint({ 0, 0 });
    eLayerInput->setMaxLabelLength(4);
    eLayerInput->setScale(.7f);
    eLayerInput->setDelegate(ad);
    eLayerInput->setUserData(reinterpret_cast<void*>(id));
    eLayerInput->setTag(id + 64);
    updateInputText(eLayerInput, ov);

    auto txt = CCLabelBMFont::create(title, "goldFont.fnt");
    txt->limitLabelWidth(85.0f, 1.0f, .3f);
    txt->setPosition(text->getPosition() - CCPoint { 35.0f, 0.0f });

    self->addChild(spr);
    self->addChild(eLayerInput);
    self->addChild(txt);
}

ccColor3B getColorChannelBaseColor(int channelID, int stackLimit = 0) {
    auto col = LevelEditorLayer::get()->m_pLevelSettings->m_effectManager->getColorAction(channelID);

    if (stackLimit > 10 || !col)
        return { 255, 255, 255 };

    if (col->m_copyID) {
        auto copy = getColorChannelBaseColor(col->m_copyID, stackLimit + 1);

        GameToolbox::transformColor(copy, copy, cchsv(
            col->m_copyHue,
            col->m_copySaturation,
            col->m_copyBrightness,
            col->m_saturationChecked,
            col->m_brightnessChecked
        ));

        return copy;
    }

    return col->m_color;
}

void updateHSVPreviewColor(HSVWidgetPopup* self) {
    auto prev = as<CCSprite*>(self->m_pLayer->getChildByTag(HSVSQUARE2_TAG));

    if (prev) {
        auto col = getColorChannelBaseColor(
            as<int>(prev->getUserData())
        );

        ccColor3B col3b;
        GameToolbox::transformColor(col, col3b, self->m_pConfigureWidget->m_obValue);

        prev->setColor(col3b);
    }
}

GDMAKE_HOOK(0x4adf0)
void __fastcall ConfigureHSVWidget_updateLabels(ConfigureHSVWidget* self) {
    GDMAKE_ORIG_V(self);

    if (self->getChildByTag(69))
        updateInputText(reinterpret_cast<CCTextInputNode*>(self->getChildByTag(69)), self->m_obValue.h);
    if (self->getChildByTag(70))
        updateInputText(reinterpret_cast<CCTextInputNode*>(self->getChildByTag(70)), self->m_obValue.s);
    if (self->getChildByTag(71))
        updateInputText(reinterpret_cast<CCTextInputNode*>(self->getChildByTag(71)), self->m_obValue.v);
    
    if (self->getUserData())
        updateHSVPreviewColor(as<HSVWidgetPopup*>(self->getUserData()));
}

GDMAKE_HOOK(0x4a3f0)
bool __fastcall ConfigureHSVWidget_init(ConfigureHSVWidget* self, edx_t edx, int abs, ccHSVValue val) {
    if (!GDMAKE_ORIG(self, edx, abs, val))
        return false;
    
    auto hsva = AddHSVTextDelegate::create(self);

    turnLabelIntoInput(
        self,
        self->m_pHueLabel,
        hsva,
        5,
        self->m_obValue.h,
        "Hue"
    );
    turnLabelIntoInput(
        self,
        self->m_pSaturationLabel,
        hsva,
        6,
        self->m_obValue.s,
        "Saturation"
    );
    turnLabelIntoInput(
        self,
        self->m_pBrightnessLabel,
        hsva,
        7,
        self->m_obValue.v,
        "Brightness"
    );

    self->addChild(hsva);

    return true;
}

GDMAKE_HOOK(0x567c0)
void __fastcall CustomizeObjectLayer_onHSV(CustomizeObjectLayer* self, edx_t edx, CCMenuItemSpriteExtra* pSender) {
    g_hsvColorChannel = self->getActiveMode(true);

    GDMAKE_ORIG(self, edx, pSender);
}

GDMAKE_HOOK(0x49f10)
bool __fastcall HSVWidgetPopup_init(HSVWidgetPopup* self, edx_t edx, HSVWidgetPopupDelegate* delegate, ccHSVValue hsv, std::string str) {
    if (!GDMAKE_ORIG(self, edx, delegate, hsv, str))
        return false;

    if (g_hsvColorChannel < 1)
        return true;
    
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto col = getColorChannelBaseColor(g_hsvColorChannel);

    auto square1 = CCSprite::createWithSpriteFrameName("whiteSquare60_001.png");

    square1->setPosition({ winSize.width / 2 - 155.0f, winSize.height / 2 - 15.0f });
    square1->setColor(col);
    square1->setTag(HSVSQUARE1_TAG);
    square1->setUserData(as<void*>(g_hsvColorChannel));

    self->m_pLayer->addChild(square1);

    auto square2 = CCSprite::createWithSpriteFrameName("whiteSquare60_001.png");

    square2->setPosition({ winSize.width / 2 - 155.0f, winSize.height / 2 + 15.0f });
    square2->setColor(col);
    square2->setTag(HSVSQUARE2_TAG);
    square2->setUserData(as<void*>(g_hsvColorChannel));

    self->m_pLayer->addChild(square2);
    self->m_pConfigureWidget->setUserData(self);

    updateHSVPreviewColor(self);

    g_hsvColorChannel = 0;

    return true;
}
