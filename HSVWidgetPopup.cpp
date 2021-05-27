#include <GDMake.h>
#include <GUI/CCControlExtension/CCScale9Sprite.h>

using namespace gdmake;

class ConfigureHSVWidget;
class HSVWidgetPopup;

struct _ccHSVValue {
    int unknown;
    int h, s, v;
};

class ConfigureHSVWidget : public cocos2d::CCNode {
    public:
        cocos2d::CCLabelBMFont* m_pHueLabel;
        cocos2d::CCLabelBMFont* m_pSaturationLabel;
        cocos2d::CCLabelBMFont* m_pBrightnessLabel;

        gd::Slider* m_pHueSlider;
        gd::Slider* m_pSaturationSlider;
        gd::Slider* m_pBrightnessSlider;

        float m_fHueValue;
        float m_fSaturationValue;
        float m_fBrightnessValue;

        bool m_bAbsoluteSaturation;
        bool m_bAbsoluteBrightness;
};

class AddHSVTextDelegate : public cocos2d::CCNode, public gd::TextInputDelegate {
    public:
        ConfigureHSVWidget* m_pGIL;

        virtual void textChanged(gd::CCTextInputNode* input) override {
            float val = 0.0f;

            if (input->getString() && strlen(input->getString()))
                val = std::atof(input->getString());

            switch (reinterpret_cast<int>(input->getUserData())) {
                case 5:
                    m_pGIL->m_pHueSlider->setValue((val + 180.0f) / 360.0f);
                    m_pGIL->m_fHueValue = val;
                    break;
                case 6:
                    if (m_pGIL->m_bAbsoluteSaturation)
                        m_pGIL->m_pSaturationSlider->setValue((val + 1.0f) / 2);
                    else
                        m_pGIL->m_pSaturationSlider->setValue(val / 2);
                    m_pGIL->m_fSaturationValue = val;
                    break;
                case 7:
                    if (m_pGIL->m_bAbsoluteBrightness)
                        m_pGIL->m_pBrightnessSlider->setValue((val + 1.0f) / 2);
                    else
                        m_pGIL->m_pBrightnessSlider->setValue(val / 2);
                    m_pGIL->m_fBrightnessValue = val;
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

class HSVWidgetPopupDelegate {
    public:
        virtual void hsvPopupClosed(HSVWidgetPopup *, _ccHSVValue);
};

class HSVWidgetPopup : public gd::FLAlertLayer {
    public:
        ConfigureHSVWidget* m_pConfigureWidget;
};

void updateInputText(gd::CCTextInputNode* upd, float val) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << val;
    std::string s = stream.str();

    upd->setString(s.c_str());
}

void turnLabelIntoInput(
    ConfigureHSVWidget* self,
    cocos2d::CCLabelBMFont* text,
    AddHSVTextDelegate* ad,
    int id,
    float ov,
    std::string const& ac = "-0123456789."
) {
    text->setVisible(false);

    auto spr = cocos2d::extension::CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );

    spr->setScale(.3f);
    spr->setColor({ 0, 0, 0 });
    spr->setOpacity(100);
    spr->setContentSize({ 235.0f, 70.0f });
    spr->setPosition(text->getPosition());

    auto eLayerInput = gd::CCTextInputNode::create("0", ad, "bigFont.fnt", 70.0f, 20.0f);

    eLayerInput->setPosition(text->getPosition());
    eLayerInput->setLabelPlaceholderColor({ 120, 120, 120 });
    eLayerInput->setAllowedChars(ac);
    eLayerInput->setAnchorPoint({ 0, 0 });
    eLayerInput->setMaxLabelLength(4);
    eLayerInput->setScale(.7f);
    eLayerInput->setDelegate(ad);
    eLayerInput->setUserData(reinterpret_cast<void*>(id));
    eLayerInput->setTag(id + 64);
    updateInputText(eLayerInput, ov);

    self->addChild(spr);
    self->addChild(eLayerInput);
}

GDMAKE_HOOK(0x4adf0)
void __fastcall ConfigureHSVWidget_updateLabels(ConfigureHSVWidget* self) {
    GDMAKE_ORIG_V(self);

    if (self->getChildByTag(69))
        updateInputText(reinterpret_cast<gd::CCTextInputNode*>(self->getChildByTag(69)), self->m_fHueValue);
    if (self->getChildByTag(70))
        updateInputText(reinterpret_cast<gd::CCTextInputNode*>(self->getChildByTag(70)), self->m_fSaturationValue);
    if (self->getChildByTag(71))
        updateInputText(reinterpret_cast<gd::CCTextInputNode*>(self->getChildByTag(71)), self->m_fBrightnessValue);
}

GDMAKE_HOOK(0x4a3f0)
bool __fastcall ConfigureHSVWidget_init(ConfigureHSVWidget* self, edx_t edx, _ccHSVValue val, bool idk) {
    if (!GDMAKE_ORIG(self, edx, val, idk))
        return false;
    
    auto hsva = AddHSVTextDelegate::create(self);

    turnLabelIntoInput(
        self,
        self->m_pHueLabel,
        hsva,
        5,
        self->m_fHueValue
    );
    turnLabelIntoInput(
        self,
        self->m_pSaturationLabel,
        hsva,
        6,
        self->m_fSaturationValue
    );
    turnLabelIntoInput(
        self,
        self->m_pBrightnessLabel,
        hsva,
        7,
        self->m_fBrightnessValue
    );

    self->addChild(hsva);

    return true;
}
