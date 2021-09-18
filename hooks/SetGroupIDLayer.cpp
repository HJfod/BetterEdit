#include <GDMake.h>
#include <GUI/CCControlExtension/CCScale9Sprite.h>
#include "../utils.hpp"
#include <InputPrompt.hpp>

using namespace gd;
using namespace gdmake;
using namespace gdmake::extra;
using namespace cocos2d;

std::string g_nextFreeInput = "";
static constexpr const int NEXTFREE_TAG = 5000;
int g_nMixedZOrder = 0;

class AddTextDelegate : public cocos2d::CCNode, public gd::TextInputDelegate {
    public:
        SetGroupIDLayer* m_pGIL;

        virtual void textChanged(gd::CCTextInputNode* input) override {
            reinterpret_cast<void(__thiscall*)(SetGroupIDLayer*, gd::CCTextInputNode*)>(
                gd::base + 0x22d610
            )(this->m_pGIL, input);
        }

        static AddTextDelegate* create(SetGroupIDLayer* gil) {
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

class SetGroupIDLayer_CB : public SetGroupIDLayer {
    public:
        void onCustomNextFree(CCObject* pSender) {
            auto p = InputPrompt::create("Next Free Offset", "ID", [this](const char* txt) -> void {
                if (txt && strlen(txt)) {
                    auto startID = 0;
                    try { startID = std::stoi(txt); } catch (...) {}

                    auto bytes = intToBytes(startID);
                    bytes.insert(bytes.begin(), 0xbe);

                    patchBytes(0x164c59, bytes);
                    auto id = LevelEditorLayer::get()->getNextFreeGroupID(nullptr);

                    this->m_nGroupIDValue = id;
                    this->updateGroupIDLabel();

                    g_nextFreeInput = txt;
                } else {
                    g_nextFreeInput = "";
                    patchBytes(0x164c59, { 0xbe, 0x01, 0x00, 0x00, 0x00 });
                }

                auto btn = as<CCMenuItemSpriteExtra*>(this->m_pButtonMenu->getChildByTag(NEXTFREE_TAG));

                if (btn) {
                    auto spr = as<ButtonSprite*>(btn->getNormalImage());
                    spr->setString(g_nextFreeInput.size() ? g_nextFreeInput.c_str() : "0");
                    spr->updateBGImage(
                        g_nextFreeInput.size() ? "GJ_button_02.png" : "GJ_button_04.png"
                    );
                }
            }, "Apply")->setApplyOnEsc(true)->setTrashButton(true);
            p->getInputNode()->getInputNode()->setAllowedChars("0123456789");
            p->getInputNode()->getInputNode()->setMaxLabelLength(6);
            p->getInputNode()->setString(g_nextFreeInput.c_str());
            p->show();
        }
};

GDMAKE_HOOK(0x22d690)
void __fastcall SetGroupIDLayer_onEditorLayer(SetGroupIDLayer* self, edx_t edx, cocos2d::CCObject* pSender) {
    GDMAKE_ORIG_V(self, edx, pSender);

    auto i = self->getChildByTag(69);

    if (i)
        reinterpret_cast<gd::CCTextInputNode*>(i)->setString(
            std::to_string(self->m_nEditorLayerValue).c_str()
        );
}

GDMAKE_HOOK(0x22d710)
void __fastcall SetGroupIDLayer_onEditorLayer2(SetGroupIDLayer* self, edx_t edx, cocos2d::CCObject* pSender) {
    GDMAKE_ORIG_V(self, edx, pSender);

    auto i = self->getChildByTag(70);

    if (i)
        reinterpret_cast<gd::CCTextInputNode*>(i)->setString(
            std::to_string(self->m_nEditorLayer2Value).c_str()
        );
}

GDMAKE_HOOK(0x22de80)
void __fastcall SetGroupIDLayer_onZOrder(SetGroupIDLayer* self, edx_t edx, cocos2d::CCObject* pSender) {
    // GDMAKE_ORIG_V(self, edx, pSender);

    auto add = pSender->getTag();
    if (self->m_nZOrderValue != -1000) {
        self->m_nZOrderValue += add;
        if (self->m_nZOrderValue == 0)
            self->m_nZOrderValue = add;
        
        if (self->m_nZOrderValue > 100)
            self->m_nZOrderValue = 100;
            
        if (self->m_nZOrderValue < -100)
            self->m_nZOrderValue = -100;
        
        self->m_pZOrderText->setString(
            CCString::createWithFormat("%i", self->m_nZOrderValue)
        );
    } else {
        g_nMixedZOrder += add;

        if (g_nMixedZOrder)
            self->m_pZOrderText->setString(
                CCString::createWithFormat("Mix+%i", g_nMixedOrder)
            );
        else
            self->m_pZOrderText->setString("Mixed");
    }

    auto i = self->getChildByTag(71);

    if (i)
        reinterpret_cast<CCTextInputNode*>(i)->setString(
            std::to_string(self->m_nZOrderValue).c_str()
        );
}

GDMAKE_HOOK(0x22d610)
void __fastcall SetGroupIDLayer_textChanged(SetGroupIDLayer* self, edx_t edx, CCTextInputNode* input) {
    int val = 0;
    bool isInt = true;

    if (input->getString() && strlen(input->getString()) > 0)
        val = strToInt(input->getString(), &isInt);

    switch (reinterpret_cast<int>(input->getUserData())) {
        case 5:
            if (!isInt) return;

            self->m_nEditorLayerValue = val;
            self->updateEditorLayerID();
            break;

        case 6:
            if (!isInt) return;
            
            self->m_nEditorLayer2Value = val;
            self->updateEditorLayerID2();
            break;

        case 7:
            if (!isInt) return;
            
            self->m_nZOrderValue = val;
            self->updateZOrder();
            break;
        
        default:
            return GDMAKE_ORIG_V(self, edx, input);
    }
}

void turnLabelIntoInput(
    SetGroupIDLayer* self,
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
    SetGroupIDLayer* self,
    edx_t edx,
    GameObject* obj,
    CCArray* arr
) {
    if (!GDMAKE_ORIG(self, edx, obj, arr))
        return false;

    g_nMixedZOrder = 0;

    auto nextFreeBtn = getChild<CCMenuItemSpriteExtra*>(self->m_pButtonMenu, 2);
    nextFreeBtn->setPositionX(nextFreeBtn->getPositionX() - 20.0f);

    auto customNextFreeBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create(
            g_nextFreeInput.size() ? g_nextFreeInput.c_str() : "0", 20, true, "goldFont.fnt", 
            g_nextFreeInput.size() ? "GJ_button_02.png" : "GJ_button_04.png", 25, .6f
        ),
        self,
        (SEL_MenuHandler)&SetGroupIDLayer_CB::onCustomNextFree
    );
    customNextFreeBtn->setTag(NEXTFREE_TAG);
    customNextFreeBtn->setPosition(
        nextFreeBtn->getPositionX() + 58.0f,
        nextFreeBtn->getPositionY()
    );
    self->m_pButtonMenu->addChild(customNextFreeBtn);

    auto inp = AddTextDelegate::create(self);

    turnLabelIntoInput(self, self->m_pEditorLayerText, inp, 5, self->m_pEditorLayerText->getString());
    turnLabelIntoInput(self, self->m_pEditorLayer2Text, inp, 6, self->m_pEditorLayer2Text->getString());
    turnLabelIntoInput(self, self->m_pZOrderText, inp, 7, self->m_pZOrderText->getString(), "-0123456789");

    self->addChild(inp);

    return true;
}
