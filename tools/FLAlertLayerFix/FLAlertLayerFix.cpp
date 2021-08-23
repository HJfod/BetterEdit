#include "../../BetterEdit.hpp"

int layer_count = 0;

GDMAKE_HOOK(0x224b0)
FLAlertLayer * __fastcall FLAlertLayer_constructor(FLAlertLayer* layer) {
    layer_count++;

    return GDMAKE_ORIG_P(layer);
}

GDMAKE_HOOK(0x225c0)
FLAlertLayer * __fastcall FLAlertLayer_destructor(FLAlertLayer* layer) {
    layer_count--;

    return GDMAKE_ORIG_P(layer);
}

void fixLayering(FLAlertLayer* layer) {
    layer->m_nZOrder2 = 100 + layer_count * 200;
    layer->setZOrder(layer->m_nZOrder2);

    std::cout << layer->m_nZOrder2 << "\n";
}

GDMAKE_HOOK(0x23560)
void __fastcall FLAlertLayer_show(FLAlertLayer* layer) {
    GDMAKE_ORIG_V(layer);
    fixLayering(layer);
}

GDMAKE_HOOK(0x46ff0)
void __fastcall ColorSelectPopup_show(ColorSelectPopup* layer) {
    GDMAKE_ORIG_V(layer);
    fixLayering(layer);
}

GDMAKE_HOOK(0x672c0)
void __fastcall CustomSongLayer_show(FLAlertLayer* layer) {
    GDMAKE_ORIG_V(layer);
    fixLayering(layer);
}

void loadFLAlertLayerFix() {}

