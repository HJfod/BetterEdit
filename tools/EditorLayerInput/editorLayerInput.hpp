#pragma once

#include "../../BetterEdit.hpp"

static constexpr const int LAYERINPUT_TAG = 6978;
static constexpr const int LAYERINPUTBG_TAG = 6977;

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

void loadEditorLayerInput(EditorUI*);
