#include "loadPasteButton.hpp"
#include <InputPrompt.hpp>

class EditorPauseLayer_CB : public EditorPauseLayer {
    public:
        void onPasteFromString(CCObject*) {
            auto p = InputPrompt::create("Paste Object String", "String", [this](const char* val) -> void {
                if (val && strlen(val)) {
                    // auto objs = this->m_pEditorLayer->m_pEditorUI->getSelectedObjects();
                    this->m_pEditorLayer->m_pEditorUI->pasteObjects(val);
                    this->m_pEditorLayer->m_pEditorUI->deselectAll();
                    // this->m_pEditorLayer->m_pEditorUI->selectObjects(objs, true);
                }
            }, "Paste");
            
            p->getInputNode()->getInputNode()->setAllowedChars(inputf_Default);
            p->show();
        }
};

void loadPasteButton(EditorPauseLayer* self) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto menu = as<CCMenu*>(self->m_pButton0->getParent());

    auto btn = CCMenuItemSpriteExtra::create(
        CCNodeConstructor()
            .fromNode(ButtonSprite::create(
                "Paste String", 0x32, true, "bigFont.fnt", "GJ_button_04.png", 30.0, .6f
            ))
            .done(),
        self,
        (SEL_MenuHandler)&EditorPauseLayer_CB::onPasteFromString
    );
    auto keysBtn = extra::getChild<CCNode*>(menu, -1);

    btn->setPosition(keysBtn->getPositionX(),
    keysBtn->getPositionY() + 35.0f);

    menu->addChild(btn);
}
