#include <GDMake.h>
#include "favourite.hpp"
#include "../../utils/addTab.hpp"

using namespace gd;
using namespace gdmake;
using namespace gdmake::extra;
using namespace cocos2d;

inline cocos2d::CCSprite* make_bspr(char c, const char* bg = "GJ_button_01.png") {
    auto spr = cocos2d::CCSprite::create(bg);

    auto label = cocos2d::CCLabelBMFont::create(std::string(1, c).c_str(), "bigFont.fnt");
    label->setPosition(spr->getContentSize().width / 2, spr->getContentSize().height / 2 + 3.0f);

    spr->addChild(label);

    return spr;
}

class EditorUI_CB : public EditorUI {
    public:
        void addFavourite(CCObject*) {
            auto objs = this->getSelectedObjects();
            
            if (!objs->count())
                return FLAlertLayer::create(
                    nullptr, "Nothing selected",
                    "OK", nullptr,
                    "Select an <cb>object</c> to favourite!"
                )->show();

            if (objs->count() > 1)
                return FLAlertLayer::create(
                    nullptr, "Too many objects",
                    "OK", nullptr,
                    "Select <co>one</c> object to favourite!"
                )->show();
        }

        void removeFavourite(CCObject*) {

        }
};

void loadFavouriteTab() {
    addEditorTab("GJ_starsIcon_001.png", [](auto self) -> EditButtonBar* {
        auto btns = CCArray::create();
        
        btns->addObject(CCMenuItemSpriteExtra::create(
            make_bspr('+'),
            self,
            (SEL_MenuHandler)&EditorUI_CB::addFavourite
        ));
        
        btns->addObject(CCMenuItemSpriteExtra::create(
            make_bspr('-', "GJ_button_06.png"),
            self,
            (SEL_MenuHandler)&EditorUI_CB::removeFavourite
        ));

        return gd::EditButtonBar::create(
            btns, { CCDirector::sharedDirector()->getWinSize().width / 2, 86.0f }, self->m_pTabsArray->count(), false,
            GameManager::sharedState()->getIntGameVariable("0049"),
            GameManager::sharedState()->getIntGameVariable("0050")
        );
    });
}
