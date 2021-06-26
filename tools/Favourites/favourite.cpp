#include <GDMake.h>
#include "favourite.hpp"
#include "../../utils/addTab.hpp"
#include "../../BetterEdit.hpp"

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
        void onSelect(CCObject* pSender) {
            this->onCreateButton(pSender);

            if (as<CreateMenuItem*>(pSender)->m_nObjectID == this->m_nSelectedCreateObjectID)
                this->enableButton(as<CreateMenuItem*>(pSender));
            else
                this->disableButton(as<CreateMenuItem*>(pSender));
        }

        void addFavorite(CCObject* pSender) {
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

            BetterEdit::sharedState()->addFavorite(
                as<GameObject*>(objs->objectAtIndex(0))->m_nObjectID
            );

            this->loadItems(as<EditButtonBar*>(as<CCNode*>(pSender)->getUserData()));
        }

        void removeFavorite(CCObject* pSender) {
            BetterEdit::sharedState()->removeFavorite(this->m_nSelectedCreateObjectID);

            this->loadItems(as<EditButtonBar*>(as<CCNode*>(pSender)->getUserData()));

            this->m_nSelectedCreateObjectID = 0;
        }

        void loadItems(EditButtonBar* bbar) {
            if (bbar->getItems()->count() > 2)
                for (auto ix = 0u; ix < bbar->getItems()->count() - 2; ix++)
                    this->m_pCreateButtonArray->removeObject(bbar->getItems()->objectAtIndex(ix));

            bbar->removeAllItems();

            for (auto id : BetterEdit::sharedState()->getFavorites()) {
                auto cbtn = this->getCreateBtn(id, 4);
                cbtn->setTarget(this, (SEL_MenuHandler)&EditorUI_CB::onSelect);

                bbar->addButton(cbtn, false);

                this->m_pCreateButtonArray->addObject(cbtn);
            }

            auto addBtn = CCMenuItemSpriteExtra::create(
                make_bspr('+'),
                this,
                (SEL_MenuHandler)&EditorUI_CB::addFavorite
            );
            addBtn->setUserData(bbar);
            bbar->addButton(addBtn, false);
            
            auto remBtn = CCMenuItemSpriteExtra::create(
                make_bspr('-', "GJ_button_06.png"),
                this,
                (SEL_MenuHandler)&EditorUI_CB::removeFavorite
            );
            remBtn->setUserData(bbar);
            bbar->addButton(remBtn);
        }
};

void loadFavouriteTab() {
    addEditorTab("GJ_bigStar_noShadow_001.png", [](auto self) -> EditButtonBar* {
        auto bbar = gd::EditButtonBar::create(
            CCArray::create(), { CCDirector::sharedDirector()->getWinSize().width / 2, 86.0f },
            self->m_pTabsArray->count(), false,
            GameManager::sharedState()->getIntGameVariable("0049"),
            GameManager::sharedState()->getIntGameVariable("0050")
        );

        as<EditorUI_CB*>(self)->loadItems(bbar);

        return bbar;
    });
}
