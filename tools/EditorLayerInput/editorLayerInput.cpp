#include "editorLayerInput.hpp"

class EditorUI_CB : public EditorUI {
    public:
        void onNextFreeEditorLayer(CCObject*) {
            auto objs = this->m_pEditorLayer->getAllObjects();
            int ix = 0;

            CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject) {
                if (obj->m_nEditorLayer > ix)
                    ix = obj->m_nEditorLayer;
                if (obj->m_nEditorLayer2 > ix)
                    ix = obj->m_nEditorLayer2;
            }

            this->m_pEditorLayer->setCurrentLayer(ix + 1);
        }
};

GDMAKE_HOOK(0x886b0)
void __fastcall EditorUI_onGoToLayer(gd::EditorUI* self, edx_t edx, cocos2d::CCObject* pSender) {
    GDMAKE_ORIG_V(self, edx, pSender);

    auto i = self->getChildByTag(LAYERINPUT_TAG);

    self->m_pCurrentLayerLabel->setVisible(false);

    if (i)
        reinterpret_cast<gd::CCTextInputNode*>(i)->setString(
            self->m_pCurrentLayerLabel->getString()
        );
}

GDMAKE_HOOK(0x8d7e0)
void __fastcall EditorUI_onGroupDown(gd::EditorUI* self, edx_t edx, cocos2d::CCObject* pSender) {
    GDMAKE_ORIG(self, edx, pSender);

    auto i = self->getChildByTag(LAYERINPUT_TAG);

    self->m_pCurrentLayerLabel->setVisible(false);

    if (i)
        reinterpret_cast<gd::CCTextInputNode*>(i)->setString(
            self->m_pCurrentLayerLabel->getString()
        );
}

GDMAKE_HOOK(0x8d780)
void __fastcall EditorUI_onGroupUp(gd::EditorUI* self, edx_t edx, cocos2d::CCObject* pSender) {
    GDMAKE_ORIG(self, edx, pSender);

    auto i = self->getChildByTag(LAYERINPUT_TAG);

    self->m_pCurrentLayerLabel->setVisible(false);

    if (i)
        reinterpret_cast<gd::CCTextInputNode*>(i)->setString(
            self->m_pCurrentLayerLabel->getString()
        );
}

GDMAKE_HOOK(0x88790)
void __fastcall EditorUI_onGoToBaseLayer(gd::EditorUI* self, edx_t edx, cocos2d::CCObject* pSender) {
    GDMAKE_ORIG(self, edx, pSender);

    auto i = self->getChildByTag(LAYERINPUT_TAG);

    self->m_pCurrentLayerLabel->setVisible(false);

    if (i)
        reinterpret_cast<gd::CCTextInputNode*>(i)->setString(
            self->m_pCurrentLayerLabel->getString()
        );
}

void loadEditorLayerInput(EditorUI* self) {
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    auto ed = EUITextDelegate::create(self);

    self->m_pCurrentLayerLabel->setVisible(false);

    auto spr = cocos2d::extension::CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );

    spr->setScale(.3f);
    spr->setColor({ 0, 0, 0 });
    spr->setOpacity(100);
    spr->setContentSize({ 115.0f, 75.0f });
    spr->setTag(LAYERINPUTBG_TAG);
    spr->setPosition(self->m_pCurrentLayerLabel->getPosition());

    auto eLayerInput = gd::CCTextInputNode::create("All", ed, "bigFont.fnt", 40.0f, 30.0f);

    eLayerInput->setPosition(self->m_pCurrentLayerLabel->getPosition());
    eLayerInput->setLabelPlaceholderColor({ 120, 120, 120 });
    eLayerInput->setAllowedChars("0123456789");
    eLayerInput->setAnchorPoint({ 0, 0 });
    eLayerInput->setScale(.7f);
    eLayerInput->setDelegate(ed);
    eLayerInput->setTag(LAYERINPUT_TAG);
    eLayerInput->setString(self->m_pCurrentLayerLabel->getString());

    self->addChild(spr);
    self->addChild(eLayerInput);

    self->addChild(ed);

    // move child at ix 17 to x: -90
    // child at 16 to -70
    // child at 15 to -13
    // m_pCurrentLayerLabel to 508
    // bg to 507
    // input to 507
    // and also move the children first so the bg and input are auto-placed

    self->m_pEditGroupBtn->getParent()->addChild(
        CCNodeConstructor<CCMenuItemSpriteExtra*>()
            .fromNode(CCMenuItemSpriteExtra::create(
                CCNodeConstructor()
                    .fromFrameName("GJ_arrow_02_001.png")
                    .scale(.6f)
                    .flipX()
                    .done(),
                self,
                (SEL_MenuHandler)&EditorUI_CB::onNextFreeEditorLayer
            ))
            .move(7.0f, -177.0f)
            .done()
    );
}
