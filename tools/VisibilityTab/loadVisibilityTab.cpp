#include "loadVisibilityTab.hpp"
#include "../../hooks/EditorPauseLayer.hpp"
#include "../RotateSaws/rotateSaws.hpp"

// for patching stuff
static constexpr const float f_96 = 96.0f;
static constexpr const float f_0  = 0.0f;

static constexpr const int VIEWBUTTONBAR_TAG = 0x234592;

bool g_bHideLDM = false;

class CCVoidPointer : public CCObject {
    public:
        CCVoidPointer(void* v)
            : m_pValue(v) {}
        void* getValue() const {return m_pValue;}

        static CCVoidPointer* create(void* v)
        {
            CCVoidPointer* pRet = new CCVoidPointer(v);
            pRet->autorelease();
            return pRet;
        }

    private:
        void* m_pValue;
};

enum VisibilityButtonType {
    kVisibilityNone,
    kVisibilityFunc,
    kVisibilityBool,
    kVisibilityBEFunc,
    kVisibilityGMVar,
    kVisibilityGMBool,
    kVisibilityAddress,
};

#define UPD_IMG(_var_, _tag_, _tvar_) \
    if (_var_) \
    as<ButtonSprite*>(as<CCMenuItemSpriteExtra*>(_var_)->getNormalImage())  \
        ->updateBGImage(_tvar_ == _tag_ ? "GJ_button_02.png" : "GJ_button_01.png");

class VisibilityToggleCallbacks : public EditorUI {
    public:
        void onToggle(CCObject* pSender) {
            auto data = as<bool*>(as<CCNode*>(pSender)->getUserData());

            *data = !as<CCMenuItemToggler*>(pSender)->isToggled();
        }

        void onToggleGMVar(CCObject* pSender) {
            auto var = as<CCString*>(as<CCNode*>(pSender)->getUserObject())->getCString();

            GameManager::sharedState()->toggleGameVariable(var);

            this->m_pEditorLayer->updateEditorMode();
        }

        void onToggleBEFunc(CCObject* pSender) {
            auto func = as<void(*)(bool)>(as<CCNode*>(pSender)->getUserData());

            func(!as<CCMenuItemToggler*>(pSender)->isToggled());
        }

        void onToggleAddr(CCObject* pSender) {
            auto addr = as<uintptr_t>(as<CCNode*>(pSender)->getUserData());
        
            if (!as<CCMenuItemToggler*>(pSender)->isToggled())
                unpatch(addr);
            else
                patch(addr, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 });
        }
};

ButtonSprite* createBtnSprite(const char* file) {
    auto spr = ButtonSprite::create(
        createBESprite(file),
        0x32, true, .6f, 0, "GJ_button_01.png", true, 0x32
    );

    return spr;
}

CCMenuItemToggler* createEditBtn(
    EditorUI* target,
    const char* file,
    SEL_MenuHandler cb,
    bool (*get)(void) = nullptr
) {
    auto sprOff = ButtonSprite::create(
        createBESprite(file),
        0x32, true, 1.0f, 0, "GJ_button_01.png", true, 0x32
    );
    sprOff->m_pSubSprite->setScale(1.25f);

    auto sprOn = ButtonSprite::create(
        createBESprite(file),
        0x32, true, 1.0f, 0, "GJ_button_02.png", true, 0x32
    );
    sprOn->m_pSubSprite->setScale(1.25f);

    auto btn = CCMenuItemToggler::create(
        sprOff, sprOn, target, cb
    );

    btn->toggle(get());
    btn->setUserData(get);
    btn->setTag(kVisibilityFunc);

    return btn;
}

CCMenuItemToggler* createEditBtn(
    EditorUI* target,
    const char* file,
    void (*set)(bool),
    bool (*get)(void) = nullptr
) {
    auto sprOff = ButtonSprite::create(
        createBESprite(file),
        0x32, true, 1.0f, 0, "GJ_button_01.png", true, 0x32
    );
    sprOff->m_pSubSprite->setScale(1.25f);

    auto sprOn = ButtonSprite::create(
        createBESprite(file),
        0x32, true, 1.0f, 0, "GJ_button_02.png", true, 0x32
    );
    sprOn->m_pSubSprite->setScale(1.25f);

    auto btn = CCMenuItemToggler::create(
        sprOff, sprOn, target,
        (SEL_MenuHandler)&VisibilityToggleCallbacks::onToggleBEFunc
    );

    btn->setUserData(set);
    btn->setUserObject(CCVoidPointer::create(get));
    btn->toggle(get());
    btn->setTag(kVisibilityBEFunc);

    return btn;
}

CCMenuItemToggler* createEditBtn(
    EditorUI* target,
    const char* file,
    bool* b = nullptr,
    const char* gmVar = nullptr
) {
    auto sprOff = ButtonSprite::create(
        createBESprite(file),
        0x32, true, 1.0f, 0, "GJ_button_01.png", true, 0x32
    );
    sprOff->m_pSubSprite->setScale(1.25f);

    auto sprOn = ButtonSprite::create(
        createBESprite(file),
        0x32, true, 1.0f, 0, "GJ_button_02.png", true, 0x32
    );
    sprOn->m_pSubSprite->setScale(1.25f);

    SEL_MenuHandler cb = nullptr;

    if (b) cb = (SEL_MenuHandler)&VisibilityToggleCallbacks::onToggle;
    if (gmVar) cb = (SEL_MenuHandler)&VisibilityToggleCallbacks::onToggleGMVar;

    auto btn = CCMenuItemToggler::create(
        sprOff, sprOn, target, cb
    );

    btn->setTag(kVisibilityNone);
    if (b) {
        btn->setUserData(as<void*>(b));
        btn->toggle(*b);
        btn->setTag(kVisibilityBool);
    }
    if (gmVar) {
        btn->setUserObject(CCString::create(gmVar));
        btn->toggle(GameManager::sharedState()->getGameVariable(gmVar));
        btn->setTag(kVisibilityGMVar);
    }

    return btn;
}

CCMenuItemToggler* createEditBtn(
    EditorUI* target,
    const char* file,
    uintptr_t addr
) {
    auto sprOff = ButtonSprite::create(
        createBESprite(file),
        0x32, true, 1.0f, 0, "GJ_button_01.png", true, 0x32
    );
    sprOff->m_pSubSprite->setScale(1.25f);

    auto sprOn = ButtonSprite::create(
        createBESprite(file),
        0x32, true, 1.0f, 0, "GJ_button_02.png", true, 0x32
    );
    sprOn->m_pSubSprite->setScale(1.25f);

    SEL_MenuHandler cb = nullptr;

    auto btn = CCMenuItemToggler::create(
        sprOff, sprOn, target,
        (SEL_MenuHandler)&VisibilityToggleCallbacks::onToggleAddr
    );

    btn->setTag(kVisibilityAddress);
    btn->setUserData(as<void*>(addr));
    btn->toggle(ispatched(addr));

    return btn;
}

void makeVisibilityPatches() {
    patch(0x768ed, intToBytes(as<int>(&f_96)), true);
    patch(0x8c9f8, intToBytes(as<int>(&f_0)), true);
    patch(0x7af6b, intToBytes(as<int>(&f_0)), true);
    patch(0x7cc7b, intToBytes(as<int>(&f_0)), true);
}

void loadVisibilityTab(EditorUI* self) {
    BetterEdit::saveGlobalBool("hide-ldm", &g_bHideLDM);

    self->m_pBuildModeBtn->setNormalImage(createBtnSprite("BE_create_01.png"));
    self->m_pBuildModeBtn->setContentSize(
        self->m_pBuildModeBtn->getNormalImage()->getScaledContentSize()
    );
    self->m_pBuildModeBtn->setPosition(-260, -93.5f);

    self->m_pEditModeBtn->setNormalImage(createBtnSprite("BE_edit_01.png"));
    self->m_pEditModeBtn->setContentSize(
        self->m_pEditModeBtn->getNormalImage()->getScaledContentSize()
    );
    self->m_pEditModeBtn->setPosition(-216.5f, -93.5f);

    self->m_pDeleteModeBtn->setNormalImage(createBtnSprite("BE_delete_01.png"));
    self->m_pDeleteModeBtn->setContentSize(
        self->m_pDeleteModeBtn->getNormalImage()->getScaledContentSize()
    );
    self->m_pDeleteModeBtn->setPosition(-260, -136.5f);

    auto vTabBtn = CCMenuItemSpriteExtra::create(
        createBtnSprite("BE_visible_01.png"),
        self,
        (SEL_MenuHandler)&EditorUI::toggleMode
    );

    vTabBtn->setTag(4);
    vTabBtn->setPosition(-216.5f, -136.5f);

    self->m_pBuildModeBtn->getParent()->addChild(vTabBtn);

    UPD_IMG(self->m_pBuildModeBtn, 2, self->m_nSelectedMode);
    UPD_IMG(self->m_pDeleteModeBtn, 1, self->m_nSelectedMode);
    UPD_IMG(self->m_pEditModeBtn, 3, self->m_nSelectedMode);
    UPD_IMG(self->m_pBuildModeBtn->getParent()->getChildByTag(4), 4, self->m_nSelectedMode);



    auto btns = CCArray::create();

    btns->addObject(createEditBtn(
        self, "BE_v_rotate.png", 
        (SEL_MenuHandler)&EditorPauseLayer_CB::onRotateSaws,
        &shouldRotateSaw
    ));
    btns->addObject(createEditBtn(
        self, "BE_v_ldm.png", &g_bHideLDM
    ));
    btns->addObject(createEditBtn(
        self, "BE_v_pulse.png",
        &BetterEdit::setPulseObjectsInEditor,
        &BetterEdit::getPulseObjectsInEditor
    ));
    btns->addObject(createEditBtn(
        self, "BE_v_prevmode.png", nullptr, "0036"
    ));
    btns->addObject(createEditBtn(
        self, "BE_v_bpm_line.png", &GameManager::sharedState()->m_bShowSongMarkers
    ));
    btns->addObject(createEditBtn(
        self, "BE_v_pos_line.png", 0x16e310
    ));
    btns->addObject(createEditBtn(
        self, "BE_v_dur_line.png", nullptr, "0058"
    ));
    btns->addObject(createEditBtn(
        self, "BE_v_eff_line.png", nullptr, "0043"
    ));

    auto buttonBar = EditButtonBar::create(
        btns,
        { CCDirector::sharedDirector()->getWinSize().width / 2, 86.0f },
        self->m_pTabsArray->count(), false,
        GameManager::sharedState()->getIntGameVariable("0049"),
        GameManager::sharedState()->getIntGameVariable("0050")
    );
    buttonBar->setTag(VIEWBUTTONBAR_TAG);
    buttonBar->setVisible(self->m_nSelectedMode == 4);

    self->addChild(buttonBar, 10);
}

void updateVisibilityTab(EditorUI* self) {
    auto bbar = as<EditButtonBar*>(self->getChildByTag(VIEWBUTTONBAR_TAG));
    
    bbar->reloadItemsInNormalSize();
    
    CCARRAY_FOREACH_B_TYPE(
        bbar->m_pButtonArray, btn, CCMenuItemToggler
    ) {
        switch (btn->getTag()) {
            case kVisibilityGMVar:
                btn->toggle(GameManager::sharedState()->getGameVariable(
                    as<CCString*>(btn->getUserObject())->getCString()
                ));
                break;

            case kVisibilityBEFunc:
                // bloody hell
                btn->toggle(
                    as<bool(*)(void)>(
                        as<CCVoidPointer*>(btn->getUserObject())->getValue()
                    )()
                );
                break;

            case kVisibilityFunc:
                if (btn->getUserData())
                    btn->toggle(
                        as<bool(*)(void)>(btn->getUserData())()
                    );
                break;
            
            case kVisibilityBool:
                btn->toggle(*as<bool*>(btn->getUserData()));
                break;
            
            case kVisibilityAddress:
                btn->toggle(ispatched(as<uintptr_t>(btn->getUserData())));
                break;
        }
    }
}

bool shouldHideLDMObject(GameObject* obj) {
    return g_bHideLDM && obj->m_bHighDetail;
}

GDMAKE_HOOK(0x7ad20)
void __fastcall EditorUI_toggleMode(EditorUI* self, edx_t edx, CCObject* pSender) {
    if (!self->m_pBuildModeBtn->getParent()->getChildByTag(4))
        return GDMAKE_ORIG_V(self, edx, pSender);

    auto tag = pSender->getTag();

    if (self->m_nSelectedMode != tag) {
        self->m_nSelectedMode = tag;

        UPD_IMG(self->m_pBuildModeBtn, 2, tag);
        UPD_IMG(self->m_pDeleteModeBtn, 1, tag);
        UPD_IMG(self->m_pEditModeBtn, 3, tag);
        UPD_IMG(self->m_pBuildModeBtn->getParent()->getChildByTag(4), 4, tag);

        // manually doing EditorUI::resetUI
        // because that replaces the sprites
        // and we don't want that

        self->m_pDeleteMenu->setVisible(tag == 1);
        self->updateDeleteMenu();
        self->updateCreateMenu(false);
        self->m_pEditButtonBar->setVisible(tag == 3);
        self->updateGridNodeSize();
        self->getChildByTag(VIEWBUTTONBAR_TAG)->setVisible(tag == 4);
    }
}
