#include "../BetterEdit.hpp"
#include <GUI/CCControlExtension/CCScale9Sprite.h>
#include "../utils.hpp"
#include <InputPrompt.hpp>

std::string g_nextFreeInput = "";
static constexpr const int NEXTFREE_TAG = 5000;
bool g_bZOrderMixed = false;
bool g_bEditorLayerMixed = false;
bool g_bEditorLayer2Mixed = false;

struct ObjectValueKeeper {
    int originalZOrder  = 0;
    int originalELayer  = 0;
    int originalELayer2 = 0;
};

std::map<GameObject*, ObjectValueKeeper> g_mStartValues;

class AddTextDelegate : public CCNode, public TextInputDelegate {
    public:
        SetGroupIDLayer* m_pGIL;

        virtual void textChanged(CCTextInputNode* input) override {
            reinterpret_cast<void(__thiscall*)(SetGroupIDLayer*, CCTextInputNode*)>(
                base + 0x22d610
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

void figureOutMixed(SetGroupIDLayer* self) {
    g_bZOrderMixed =        self->m_nZOrderValue == -1000;
    g_bEditorLayerMixed =   self->m_nEditorLayerValue == -1;
    g_bEditorLayer2Mixed =  self->m_nEditorLayer2Value == -1;

    if (g_bZOrderMixed)         self->m_nZOrderValue = 0;
    if (g_bEditorLayerMixed)    self->m_nEditorLayerValue = 0;
    if (g_bEditorLayer2Mixed)   self->m_nEditorLayer2Value = 0;

    g_mStartValues.clear();
    CCARRAY_FOREACH_B_TYPE(self->m_pObjs, obj, GameObject) {
        g_mStartValues[obj] = {
            obj->m_nGameZOrder,
            obj->m_nEditorLayer,
            obj->m_nEditorLayer2
        };
    }
}

void clampZOrder(int & val) {
    if (val < -100)
        val = -100;
        
    if (val > 100)
        val = 100;
}

void clampELayer(int & val) {
    if (val < 0)
        val = 0;

    // since layer locking doesn't support
    // anything past 500, maybe i should
    // limit this aswell. but idk
}

void updateInput(bool many, bool mixed, CCLabelBMFont* label, int val) {
    if (many || !mixed) {
        label->setString(
            CCString::createWithFormat("%i", val)->getCString()
        );
    } else {
        if (val) {
            if (val > 0) {
                label->setString(
                    CCString::createWithFormat("Mix+%i", val)->getCString()
                );
            } else {
                label->setString(
                    CCString::createWithFormat("Mix%i", val)->getCString()
                );
            }
        } else {
            label->setString("Mixed");
        }
    }
}

GDMAKE_HOOK(0x22d690)
void __fastcall SetGroupIDLayer_onEditorLayer(SetGroupIDLayer* self, edx_t edx, CCObject* pSender) {
    self->m_nEditorLayerValue += pSender->getTag() ? 1 : -1;
    
    if (!g_bEditorLayerMixed)
        clampELayer(self->m_nEditorLayerValue);
    
    updateInput(
        self->m_pObj,
        g_bEditorLayerMixed,
        self->m_pEditorLayerText,
        self->m_nEditorLayerValue
    );

    CATCH_NULL(as<CCTextInputNode*>(self->getChildByTag(69)))->setString(
        self->m_pEditorLayerText->getString()
    );
}

GDMAKE_HOOK(0x22e0b0)
void __fastcall SetGroupIDLayer_updateEditorLayerID(SetGroupIDLayer* self) {
    if (self->m_pObj) {
        self->m_pObj->m_nEditorLayer = self->m_nEditorLayerValue;
        clampELayer(self->m_pObj->m_nEditorLayer);
    }
    CCARRAY_FOREACH_B_TYPE(self->m_pObjs, obj, GameObject) {
        if (g_bEditorLayerMixed) {
            obj->m_nEditorLayer = g_mStartValues[obj].originalELayer +
                self->m_nEditorLayerValue;
        } else {
            obj->m_nEditorLayer = self->m_nEditorLayerValue;
        }
        clampELayer(obj->m_nEditorLayer);
    }
}

GDMAKE_HOOK(0x22d710)
void __fastcall SetGroupIDLayer_onEditorLayer2(SetGroupIDLayer* self, edx_t edx, CCObject* pSender) {
    self->m_nEditorLayer2Value += pSender->getTag() ? 1 : -1;
    
    if (!g_bEditorLayer2Mixed)
        clampELayer(self->m_nEditorLayer2Value);

    updateInput(
        self->m_pObj,
        g_bEditorLayer2Mixed,
        self->m_pEditorLayer2Text,
        self->m_nEditorLayer2Value
    );

    CATCH_NULL(as<CCTextInputNode*>(self->getChildByTag(70)))->setString(
        self->m_pEditorLayer2Text->getString()
    );
}

GDMAKE_HOOK(0x22e110)
void __fastcall SetGroupIDLayer_updateEditorLayerID2(SetGroupIDLayer* self) {
    if (self->m_pObj) {
        self->m_pObj->m_nEditorLayer2 = self->m_nEditorLayer2Value;
        clampELayer(self->m_pObj->m_nEditorLayer2);
    }
    CCARRAY_FOREACH_B_TYPE(self->m_pObjs, obj, GameObject) {
        if (g_bEditorLayer2Mixed) {
            obj->m_nEditorLayer2 = g_mStartValues[obj].originalELayer2 +
                self->m_nEditorLayer2Value;
        } else {
            obj->m_nEditorLayer2 = self->m_nEditorLayer2Value;
        }
        clampELayer(obj->m_nEditorLayer2);
    }
}

GDMAKE_HOOK(0x22de80)
void __fastcall SetGroupIDLayer_onZOrder(SetGroupIDLayer* self, edx_t edx, CCObject* pSender) {
    auto add = pSender->getTag() ? 1 : -1;
    self->m_nZOrderValue += add;
    
    // if (self->m_nZOrderValue == 0)
    //     self->m_nZOrderValue = add;
    
    updateInput(
        self->m_pObj,
        g_bZOrderMixed,
        self->m_pZOrderText,
        self->m_nZOrderValue
    );

    CATCH_NULL(as<CCTextInputNode*>(self->getChildByTag(71)))->setString(
        self->m_pZOrderText->getString()
    );
}

GDMAKE_HOOK(0x22e3d0)
void __fastcall SetGroupIDLayer_updateZOrder(SetGroupIDLayer* self) {
    if (self->m_pObj) {
        self->m_pObj->m_nGameZOrder = self->m_nZOrderValue;
        self->m_pObj->m_bUnknownLayerRelated = true;
        clampZOrder(self->m_pObj->m_nGameZOrder);
    }

    CCARRAY_FOREACH_B_TYPE(self->m_pObjs, obj, GameObject) {
        if (g_bZOrderMixed) {
            obj->m_nGameZOrder = g_mStartValues[obj].originalZOrder + self->m_nZOrderValue;
        } else {
            obj->m_nGameZOrder = self->m_nZOrderValue;
        }
        clampZOrder(obj->m_nGameZOrder);
        obj->m_bUnknownLayerRelated = true;
    }
}

GDMAKE_HOOK(0x22d610)
void __fastcall SetGroupIDLayer_textChanged(SetGroupIDLayer* self, edx_t edx, CCTextInputNode* input) {
    int val = 0;
    bool isInt = true;
    bool mixed = false;

    if (input->getString() && strlen(input->getString())) {
        try {
            std::string str = input->getString();
            if (stringToLower(str).find("m") != std::string::npos) {
                mixed = true;
            }
            while (str.size() && (!isdigit(str[0]) && str[0] != '-'))
                str = str.substr(1);
            if (str.size())
                val = std::stoi(str);
        } catch (...) {
            isInt = false;
        }
    }

    switch (reinterpret_cast<int>(input->getUserData())) {
        case 5:
            if (!isInt) return;
            if (!mixed) g_bEditorLayerMixed = false;
            self->m_nEditorLayerValue = val;
            self->updateEditorLayerID();
            break;

        case 6:
            if (!isInt) return;
            if (!mixed) g_bEditorLayer2Mixed = false;
            self->m_nEditorLayer2Value = val;
            self->updateEditorLayerID2();
            break;

        case 7:
            if (!isInt) return;
            if (!mixed) g_bZOrderMixed = false;
            self->m_nZOrderValue = val;
            self->updateZOrder();
            break;
        
        default:
            return GDMAKE_ORIG_V(self, edx, input);
    }
}

void turnLabelIntoInput(
    SetGroupIDLayer* self,
    CCLabelBMFont* text,
    AddTextDelegate* ad,
    int id,
    const char* ov,
    std::string const& ac = "0123456789"
) {
    text->setVisible(false);

    auto spr = extension::CCScale9Sprite::create(
        "square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f }
    );

    spr->setScale(.5f);
    spr->setColor({ 0, 0, 0 });
    spr->setOpacity(100);
    spr->setContentSize({ 80.0f, 60.0f });
    spr->setPosition(text->getPosition());

    auto eLayerInput = CCTextInputNode::create("0", ad, "bigFont.fnt", 50.0f, 40.0f);

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

    figureOutMixed(self);

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
    turnLabelIntoInput(self, self->m_pZOrderText, inp, 7, self->m_pZOrderText->getString(), "MIXEDmixed-+0123456789");

    self->addChild(inp);

    return true;
}
