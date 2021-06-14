#include <GDMake.h>
#include "TemplateMenu.hpp"
#include "../utils/addTab.hpp"
#include "templates.hpp"

using namespace gdmake;

class EditorUI_CB : public gd::EditorUI {
    public:
        void showTemplateMenu(cocos2d::CCObject*) {
            extra::as<TemplateMenu*>(this->m_pCreateButtonBar)->switchMode(TemplateMenu::kTModeEdit);
        }
};

void loadTemplates() {
    addEditorTab("puzzle_base_001.png", [](auto self) -> gd::EditButtonBar* {
        auto btns = cocos2d::CCArray::create();
        
        btns->addObject(gd::CCMenuItemSpriteExtra::create(
            make_bspr('+'),
            self,
            (cocos2d::SEL_MenuHandler)&EditorUI_CB::showTemplateMenu
        ));

        btns->addObject(gd::CCMenuItemSpriteExtra::create(
            cocos2d::CCSprite::createWithSpriteFrameName("accountBtn_settings_001.png"),
            self,
            nullptr
        ));

        return TemplateMenu::create(
            btns, { cocos2d::CCDirector::sharedDirector()->getWinSize().width / 2, 86.0f }, self->m_pTabsArray->count(), false,
            gd::GameManager::sharedState()->getIntGameVariable("0049"),
            gd::GameManager::sharedState()->getIntGameVariable("0050")
        );
    });
}
