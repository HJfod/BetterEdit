#include "loadVisibilityTab.hpp"
#include "../../hooks/EditorPauseLayer.hpp"
#include "../RotateSaws/rotateSaws.hpp"
#include "VisibilityToggle.hpp"

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

void makeVisibilityPatches() {
    if (BetterEdit::getDisableVisibilityTab()) {
        unpatch(0x768ed);
        unpatch(0x8c9f8);
        unpatch(0x7af6b);
        unpatch(0x7cc7b);

        unpatch(0x16e218);

        return;
    }

    patch(0x768ed, intToBytes(as<int>(&f_96)), true);
    patch(0x8c9f8, intToBytes(as<int>(&f_0)), true);
    patch(0x7af6b, intToBytes(as<int>(&f_0)), true);
    patch(0x7cc7b, intToBytes(as<int>(&f_0)), true);

    patch(0x16e218, {
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
    }, true);
}

void loadVisibilityTab(EditorUI* self) {
    BetterEdit::saveGlobalBool("hide-ldm", &g_bHideLDM);

    if (BetterEdit::getDisableVisibilityTab())
        return;

    static constexpr const float item_sep = 43.0f;

    auto scr = self->m_pBuildModeBtn->getParent()->convertToNodeSpace({
        CCDirector::sharedDirector()->getScreenLeft() + 25.0f,
        CCDirector::sharedDirector()->getScreenBottom() + 23.0f
    });

    self->m_pBuildModeBtn->setNormalImage(createBtnSprite("BE_create_01.png"));
    self->m_pBuildModeBtn->setContentSize(
        self->m_pBuildModeBtn->getNormalImage()->getScaledContentSize()
    );
    self->m_pBuildModeBtn->setPosition(scr.x, scr.y + item_sep);

    self->m_pEditModeBtn->setNormalImage(createBtnSprite("BE_edit_01.png"));
    self->m_pEditModeBtn->setContentSize(
        self->m_pEditModeBtn->getNormalImage()->getScaledContentSize()
    );
    self->m_pEditModeBtn->setPosition(scr.x + item_sep, scr.y + item_sep);

    self->m_pDeleteModeBtn->setNormalImage(createBtnSprite("BE_delete_01.png"));
    self->m_pDeleteModeBtn->setContentSize(
        self->m_pDeleteModeBtn->getNormalImage()->getScaledContentSize()
    );
    self->m_pDeleteModeBtn->setPosition(scr.x, scr.y);

    auto vTabBtn = CCMenuItemSpriteExtra::create(
        createBtnSprite("BE_visible_01.png"),
        self,
        (SEL_MenuHandler)&EditorUI::toggleMode
    );

    vTabBtn->setTag(4);
    vTabBtn->setPosition(scr.x + item_sep, scr.y);

    self->m_pBuildModeBtn->getParent()->addChild(vTabBtn);

    UPD_IMG(self->m_pBuildModeBtn, 2, self->m_nSelectedMode);
    UPD_IMG(self->m_pDeleteModeBtn, 1, self->m_nSelectedMode);
    UPD_IMG(self->m_pEditModeBtn, 3, self->m_nSelectedMode);
    UPD_IMG(self->m_pBuildModeBtn->getParent()->getChildByTag(4), 4, self->m_nSelectedMode);



    auto btns = CCArray::create();

    btns->addObject(VisibilityToggle::create(
        "BE_v_rotate.png",
        []() -> bool {
            return shouldRotateSaw();
        },
        [&](bool s, auto p) -> void {
            reinterpret_cast<EditorPauseLayer_CB*>(self)
                ->onRotateSaws(p);
        }
    ));

    btns->addObject(VisibilityToggle::create(
        "BE_v_ldm.png",
        []() -> bool { return g_bHideLDM; },
        [&](bool b, auto) -> void { g_bHideLDM = b; }
    ));

    // btns->addObject(VisibilityToggle::create(
    //     "BE_v_pulse.png",
    //     []() -> bool { return BetterEdit::getPulseObjectsInEditor(); },
    //     [](bool b, auto) -> void { BetterEdit::setPulseObjectsInEditor(b); }
    // ));

    btns->addObject(VisibilityToggle::create(
        "BE_v_prevmode.png",
        [self]() -> bool {
            return GameManager::sharedState()->getGameVariable("0036");
        },
        [self](bool b, auto) -> void {
            GameManager::sharedState()->setGameVariable("0036", b);

            self->m_pEditorLayer->updateEditorMode();
        }
    ));

    btns->addObject(VisibilityToggle::create(
        "BE_v_bpm_line.png",
        [self]() -> bool {
            return GameManager::sharedState()->m_bShowSongMarkers;
        },
        [self](bool b, auto) -> void {
            GameManager::sharedState()->m_bShowSongMarkers = b;
        }
    ));

    btns->addObject(VisibilityToggle::create(
        "BE_v_pos_line.png",
        [self]() -> bool {
            return ispatched(0x16e310);
        },
        [self](bool b, auto) -> void {
            if (b)
                unpatch(0x16e310);
            else
                patch(0x16e310, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 });
        }
    )->invokeSetter());

    btns->addObject(VisibilityToggle::create(
        "BE_v_dur_line.png",
        [self]() -> bool {
            return GameManager::sharedState()->getGameVariable("0058");
        },
        [self](bool b, auto) -> void {
            GameManager::sharedState()->setGameVariable("0058", b);

            self->m_pEditorLayer->updateOptions();
        }
    ));

    btns->addObject(VisibilityToggle::create(
        "BE_v_eff_line.png",
        [self]() -> bool {
            return GameManager::sharedState()->getGameVariable("0043");
        },
        [self](bool b, auto) -> void {
            GameManager::sharedState()->setGameVariable("0043", b);

            self->m_pEditorLayer->updateOptions();
        }
    ));

    btns->addObject(VisibilityToggle::create(
        "BE_v_grnd.png",
        [self]() -> bool {
            return GameManager::sharedState()->getGameVariable("0037");
        },
        [self](bool b, auto) -> void {
            GameManager::sharedState()->setGameVariable("0037", b);

            self->m_pEditorLayer->updateOptions();
            self->m_pEditorLayer->m_pGroundLayer->setVisible(b);
        }
    ));

    btns->addObject(VisibilityToggle::create(
        "BE_v_grid.png",
        [self]() -> bool {
            return GameManager::sharedState()->getGameVariable("0038");
        },
        [self](bool b, auto) -> void {
            GameManager::sharedState()->setGameVariable("0038", b);

            self->m_pEditorLayer->updateOptions();
        }
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
        bbar->m_pButtonArray, btn, VisibilityToggle
    ) {
        btn->updateState();
    }
}

bool shouldHideLDMObject(GameObject* obj) {
    return g_bHideLDM && obj->m_bHighDetail;
}

void showVisibilityTab(EditorUI* self, bool show) {
    CATCH_NULL(as<EditButtonBar*>(self->getChildByTag(VIEWBUTTONBAR_TAG)))
        ->setVisible(show && self->m_nSelectedMode == 4);
    
    CATCH_NULL(self->m_pBuildModeBtn->getParent()->getChildByTag(4))
        ->setVisible(show);
}

GDMAKE_HOOK(0x7ad20)
void __fastcall EditorUI_toggleMode(EditorUI* self, edx_t edx, CCObject* pSender) {
    if (!self->m_pBuildModeBtn->getParent()->getChildByTag(4) ||
        BetterEdit::getDisableVisibilityTab())
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
