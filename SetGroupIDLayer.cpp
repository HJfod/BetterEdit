#include <GDMake.h>
#include <GUI/CCControlExtension/CCScale9Sprite.h>
#include "utils.hpp"

using namespace gdmake;

class AddTextDelegate : public cocos2d::CCNode, public gd::TextInputDelegate {
    public:
        gd::SetGroupIDLayer* m_pGIL;

        virtual void textChanged(gd::CCTextInputNode* input) override {
            reinterpret_cast<void(__thiscall*)(gd::SetGroupIDLayer*, gd::CCTextInputNode*)>(
                gd::base + 0x22d610
            )(this->m_pGIL, input);
        }

        static AddTextDelegate* create(gd::SetGroupIDLayer* gil) {
            auto ret = new AddTextDelegate();

            if (ret && ret->init()) {
                ret->m_pGIL = gil;
                ret->autorelease();
                return ret;
            }

            CC_SAFE_DELETE(ret);
            return nullptr;
        }
};

GDMAKE_HOOK(0x22d690)
void __fastcall SetGroupIDLayer_onEditorLayer(gd::SetGroupIDLayer* self, edx_t edx, cocos2d::CCObject* pSender) {
    GDMAKE_ORIG_V(self, edx, pSender);

    auto i = self->getChildByTag(69);

    if (i)
        reinterpret_cast<gd::CCTextInputNode*>(i)->setString(
            std::to_string(self->m_nEditorLayer).c_str()
        );
}

GDMAKE_HOOK(0x22d710)
void __fastcall SetGroupIDLayer_onEditorLayer2(gd::SetGroupIDLayer* self, edx_t edx, cocos2d::CCObject* pSender) {
    GDMAKE_ORIG_V(self, edx, pSender);

    auto i = self->getChildByTag(70);

    if (i)
        reinterpret_cast<gd::CCTextInputNode*>(i)->setString(
            std::to_string(self->m_nEditorLayer2).c_str()
        );
}

GDMAKE_HOOK(0x22de80)
void __fastcall SetGroupIDLayer_onZOrder(gd::SetGroupIDLayer* self, edx_t edx, cocos2d::CCObject* pSender) {
    GDMAKE_ORIG_V(self, edx, pSender);

    auto i = self->getChildByTag(71);

    if (i)
        reinterpret_cast<gd::CCTextInputNode*>(i)->setString(
            std::to_string(self->m_nZOrder).c_str()
        );
}

GDMAKE_HOOK(0x22d610)
void __fastcall SetGroupIDLayer_textChanged(gd::SetGroupIDLayer* self, edx_t edx, gd::CCTextInputNode* input) {
    int val = 0;
    bool isInt = true;

    if (input->getString() && strlen(input->getString()) > 0)
        val = strToInt(input->getString(), &isInt);

    switch (reinterpret_cast<int>(input->getUserData())) {
        case 5:
            if (!isInt) return;

            self->m_nEditorLayer = val;
            self->updateEditorLayerID();
            break;

        case 6:
            if (!isInt) return;
            
            self->m_nEditorLayer2 = val;
            self->updateEditorLayerID2();
            break;

        case 7:
            if (!isInt) return;
            
            self->m_nZOrder = val;
            self->updateZOrder();
            break;
        
        default:
            return GDMAKE_ORIG_V(self, edx, input);
    }
}

void turnLabelIntoInput(
    gd::SetGroupIDLayer* self,
    cocos2d::CCLabelBMFont* text,
    AddTextDelegate* ad,
    int id,
    const char* ov,
    std::string const& ac = "0123456789"
) {
    text->setVisible(false);

    auto spr = cocos2d::extension::CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );

    spr->setScale(.5f);
    spr->setColor({ 0, 0, 0 });
    spr->setOpacity(100);
    spr->setContentSize({ 80.0f, 60.0f });
    spr->setPosition(text->getPosition());

    auto eLayerInput = gd::CCTextInputNode::create("0", ad, "bigFont.fnt", 50.0f, 40.0f);

    eLayerInput->setPosition(text->getPosition());
    eLayerInput->setLabelPlaceholderColor({ 120, 120, 120 });
    eLayerInput->setAllowedChars(ac);
    eLayerInput->setAnchorPoint({ 0, 0 });
    eLayerInput->setScale(.7f);
    eLayerInput->setDelegate(ad);
    eLayerInput->setUserData(reinterpret_cast<void*>(id));
    eLayerInput->setTag(id + 64);
    eLayerInput->setString(ov);

    self->addChild(spr);
    self->addChild(eLayerInput);
}

GDMAKE_HOOK(0x22b670)
bool __fastcall SetGroupIDLayer_initHook(
    gd::SetGroupIDLayer* self,
    edx_t edx,
    gd::GameObject* obj,
    cocos2d::CCArray* arr
) {
    if (!GDMAKE_ORIG(self, edx, obj, arr))
        return false;

    auto inp = AddTextDelegate::create(self);

    turnLabelIntoInput(self, self->m_pEditorLayerText, inp, 5, self->m_pEditorLayerText->getString());
    turnLabelIntoInput(self, self->m_pEditorLayer2Text, inp, 6, self->m_pEditorLayer2Text->getString());
    turnLabelIntoInput(self, self->m_pZOrderText, inp, 7, self->m_pZOrderText->getString(), "-0123456789");

    self->addChild(inp);

    return true;
}
