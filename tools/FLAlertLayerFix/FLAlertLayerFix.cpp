#include "../../BetterEdit.hpp"

void fixLayering(CCNode* node) {
    auto target = CCDirector::sharedDirector()->getRunningScene();

    int layer_count = target->getChildrenCount();

    node->setZOrder(100 + layer_count * 200);
}

void fixLayering(FLAlertLayer* layer) {
    auto target = layer->m_pTargetLayer ?
        layer->m_pTargetLayer : 
        CCDirector::sharedDirector()->getRunningScene();

    int layer_count = target->getChildrenCount();

    layer->m_nZOrder2 = 100 + layer_count * 200;
    layer->setZOrder(layer->m_nZOrder2);
}

GDMAKE_HOOK(0x23560, "_ZN12FLAlertLayer4showEv")
void __fastcall FLAlertLayer_show(FLAlertLayer* layer) {
    GDMAKE_ORIG_V(layer);
    fixLayering(layer);
}

GDMAKE_HOOK(0x46ff0, "_ZN16ColorSelectPopup4showEv")
void __fastcall ColorSelectPopup_show(ColorSelectPopup* layer) {
    GDMAKE_ORIG_V(layer);
    fixLayering(layer);
}

GDMAKE_HOOK(0x672c0, "_ZN15CustomSongLayer4showEv")
void __fastcall CustomSongLayer_show(FLAlertLayer* layer) {
    GDMAKE_ORIG_V(layer);
    fixLayering(layer);
}

void loadFLAlertLayerFix() {}

