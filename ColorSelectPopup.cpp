#include <GDMake.h>
#include <GUI/CCControlExtension/CCControlColourPicker.h>

using namespace gdmake;

#include "RGBColorInputWidget.hpp"

RGBColorInputWidget* g_widget = nullptr;

GDMAKE_HOOK(0x43ae0)
bool __fastcall ColorSelectPopup_init(
    ColorSelectPopup* self,
    edx_t edx,
    EffectGameObject* eff_obj,
    cocos2d::CCArray* arr,
    ColorAction* action
) {
    if (!GDMAKE_ORIG(self, edx, eff_obj, arr, action))
        return false;

    auto layer = self->getAlertLayer();
    auto widget = RGBColorInputWidget::create(self);

    g_widget = widget;

    auto center = cocos2d::CCDirector::sharedDirector()->getWinSize() / 2;
    if (self->isColorTrigger)
        widget->setPosition({center.width - 155.f, center.height + 29.f});
    else
        widget->setPosition({center.width + 127.f, center.height - 90.f});
    layer->addChild(widget);

    widget->setVisible(!self->copyColor);

    return true;
}

GDMAKE_HOOK(0x46f30)
void __fastcall ColorSelectPopup_colorValueChanged(ColorSelectPopup* self) {
    GDMAKE_ORIG_V(self);

    if (g_widget)
        g_widget->update_labels(true, true);
}

GDMAKE_HOOK(0x479c0)
void __fastcall ColorSelectPopup_updateCopyColorIdfk(ColorSelectPopup* self) {
    GDMAKE_ORIG_V(self);

    if (g_widget)
        g_widget->setVisible(!self->copyColor);
}

GDMAKE_HOOK(0x43900)
void __fastcall ColorSelectPopup_dtor(void* self) {
    g_widget = nullptr;

    GDMAKE_ORIG_V(self);
}

