#include "../../BetterEdit.hpp"
#include "RGBColorInputWidget.hpp"
#include "../EyeDropper/eyeDropper.hpp"

static constexpr const int WIDGET_TAG = 69696;

GDMAKE_HOOK(0x43ae0)
bool __fastcall ColorSelectPopup_init(
    ColorSelectPopup* self,
    edx_t edx,
    EffectGameObject* eff_obj,
    CCArray* arr,
    ColorAction* action
) {
    if (!GDMAKE_ORIG(self, edx, eff_obj, arr, action))
        return false;
    
    auto layer = self->getAlertLayer();

    auto widget = RGBColorInputWidget::create(self);
    widget->setTag(WIDGET_TAG);

    auto center = CCDirector::sharedDirector()->getWinSize() / 2.f;

    if (self->isColorTrigger)
        widget->setPosition({center.width - 155.f, center.height + 29.f});
    else
        widget->setPosition({center.width + 127.f, center.height - 90.f});

    layer->addChild(widget);
    widget->setVisible(!self->copyColor);

    loadEyeDropper(self);

    return true;
}

GDMAKE_HOOK(0x46f30)
void __fastcall ColorSelectPopup_colorValueChanged(ColorSelectPopup* self) {
    GDMAKE_ORIG(self);
    CATCH_NULL(as<RGBColorInputWidget*>(self->getAlertLayer()->getChildByTag(WIDGET_TAG)))
        ->update_labels(true, true);
}

GDMAKE_HOOK(0x479c0)
void __fastcall ColorSelectPopup_updateCopyColorIdfk(ColorSelectPopup* self) {
    GDMAKE_ORIG(self);
    CATCH_NULL(as<RGBColorInputWidget*>(self->getAlertLayer()->getChildByTag(WIDGET_TAG)))
        ->setVisible(!self->copyColor);
    showEyeDropper(self);
}

GDMAKE_HOOK(0x23e980)
bool __fastcall SetupPulsePopup_init(SetupPulsePopup* self, edx_t edx, EffectGameObject* eff_obj, CCArray* arr) {
    if (!GDMAKE_ORIG(self, edx, eff_obj, arr))
        return false;

    auto layer = as<ColorSelectPopup*>(self)->getAlertLayer();

    auto widget = RGBColorInputWidget::create(as<ColorSelectPopup*>(self));
        // the color picker is in the same offset in both classe s
    widget->setTag(WIDGET_TAG);

    auto center = CCDirector::sharedDirector()->getWinSize() / 2.f;

    self->colorPicker->setPositionX(self->colorPicker->getPositionX() + 3.7f);
    widget->setPosition({center.width - 132.f, center.height + 32.f});

    auto square_width = self->currentColorSpr->getScaledContentSize().width;
    auto x = widget->getPositionX() - square_width / 2.f;

    self->currentColorSpr->setPosition({x + 20.0f, center.height + 85.f});
    self->prevColorSpr->setPosition({x + 20.0f + square_width, center.height + 85.f});

    layer->addChild(widget);
    widget->setVisible(self->pulseMode == 0);

    loadEyeDropper(self);

    return true;
}

GDMAKE_HOOK(0x2426b0)
void __fastcall SetupPulsePopup_colorValueChanged(SetupPulsePopup* self) {
    GDMAKE_ORIG_V(self);
    CATCH_NULL(as<RGBColorInputWidget*>(as<ColorSelectPopup*>(self)->getAlertLayer()->getChildByTag(WIDGET_TAG)))
        ->update_labels(true, true);
}

GDMAKE_HOOK(0x242cf0)
void __fastcall SetupPulsePopup_switchPulseModeIDK(SetupPulsePopup* self) {
    GDMAKE_ORIG_V(self);
    CATCH_NULL(as<RGBColorInputWidget*>(as<ColorSelectPopup*>(self)->getAlertLayer()->getChildByTag(WIDGET_TAG)))
        ->setVisible(self->pulseMode == 0);
    showEyeDropper(self);
}
