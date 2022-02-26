#pragma once

#include "../../BetterEdit.hpp"

static constexpr const int LAYERINPUT_TAG = 6978;
static constexpr const int LAYERINPUTBG_TAG = 6977;
static constexpr const int NEXTFREELAYER_TAG = 7001;
static constexpr const int LOCKLAYER_TAG = 7002;
static constexpr const int VIEWLAYERS_TAG = 7003;

class EUITextDelegate : public cocos2d::CCNode, public gd::TextInputDelegate {
    public:
        gd::EditorUI* m_pEditorUI;

        virtual void textChanged(gd::CCTextInputNode* input) override {
            if (input->getString() && strlen(input->getString()))
                this->m_pEditorUI->m_pEditorLayer->setCurrentLayer(strToInt(input->getString()));
            else
                this->m_pEditorUI->m_pEditorLayer->setCurrentLayer(-1);
        }

        static EUITextDelegate* create(gd::EditorUI* ui) {
            auto ret = new EUITextDelegate();

            if (ret && ret->init()) {
                ret->m_pEditorUI = ui;
                ret->autorelease();
                return ret;
            }

            CC_SAFE_DELETE(ret);
            return nullptr;
        }
};

class EditorUI_CB : public EditorUI {
    public:
        void onNextFreeEditorLayer(CCObject*);
        void onLockLayer(CCObject*);
        void onShowLayerPopup(CCObject*);
        void onToggleShowUI(CCObject*);
        void onExitViewMode(CCObject*);
        void onGoToPercentage(CCObject* pSender);
};

void showLayerControls(EditorUI*, bool);
void updateEditorLayerInputText(EditorUI*);
void loadEditorLayerInput(EditorUI*);
bool testSelectObjectLayer(GameObject*);
