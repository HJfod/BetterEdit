#include <Geode/ui/Scrollbar.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/Bindings.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/modify/SelectFontLayer.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/ui/Scrollbar.hpp>
#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

class SelFont {
public:
    //this shits not working in fields :(
    static inline std::vector<CCMenuItemToggler*> buttons = {};
};

class $modify(BetterSelectFontLayer, SelectFontLayer) {
    void onSelect(CCObject* sender) {
        size_t s = static_cast<CCNode*>(sender)->getTag();

        GameManager::get()->m_levelEditorLayer->updateLevelFont(s);

        for (size_t i = 0; i < SelFont::buttons.size(); i++) {
            if (i != s) {
                SelFont::buttons[i]->toggle(false);    
                SelFont::buttons[i]->setEnabled(true);
            }
            else {
                SelFont::buttons[i]->toggle(true);
                SelFont::buttons[i]->setEnabled(false);
            }
        }
    }

    $override
    bool init(LevelEditorLayer* p0) {
        if (!SelectFontLayer::init(p0))
            return false;

        if (!Mod::get()->getSettingValue<bool>("better-font-select"))
            return true;

        //position old stuff and hide

        static_cast<CCScale9Sprite*>(static_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0))->setContentSize(ccp(400, 290));
        static_cast<CCLayer*>(this->getChildren()->objectAtIndex(1))->setPositionY(290);
        static_cast<CCLayer*>(this->getChildren()->objectAtIndex(2))->setVisible(false);

        auto butMenu = static_cast<CCMenu*>(static_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(1));
        static_cast<CCNode*>(butMenu->getChildren()->objectAtIndex(0))->setPositionY(-45);
        static_cast<CCNode*>(butMenu->getChildren()->objectAtIndex(1))->setPosition(ccp(-182, 200));
        static_cast<CCNode*>(butMenu->getChildren()->objectAtIndex(2))->setVisible(false);
        static_cast<CCNode*>(butMenu->getChildren()->objectAtIndex(3))->setVisible(false);

        auto scroll = ScrollLayer::create(ccp(320, 210));
        scroll->setAnchorPoint(ccp(0, 0));
        scroll->setPosition(CCDirector::get()->getWinSize() / 2 - scroll->getContentSize() / 2 + ccp(0, 10));

        auto bar = Scrollbar::create(scroll);
        bar->setPosition(ccp(175, 10) + CCDirector::sharedDirector()->getWinSize() / 2);
        this->addChild(bar);

        auto content = scroll->m_contentLayer;

        constexpr size_t FONT_COUNT = 60;
        content->setContentSize(ccp(320, 30 * (FONT_COUNT)));

        auto unSel = CCSprite::createWithSpriteFrameName("GJ_selectSongBtn_001.png");
        auto sel = CCSprite::createWithSpriteFrameName("GJ_selectSongOnBtn_001.png");
        unSel->setScale(0.6f);
        sel->setScale(0.6f);

        SelFont::buttons = {};

        for (size_t i = 0; i < FONT_COUNT; i++) {
            auto node = CCLayerColor::create();
            node->setColor({0, 0, 0});
            node->setOpacity((i % 2 == 0) ? 100 : 50);
            node->setContentSize(ccp(320, 30));
            node->setPosition(ccp(0, 30 * FONT_COUNT - ((i + 1) * 30)));

            auto menu = CCMenu::create();
            menu->setPosition(ccp(0, 0));
            menu->setContentSize({0, 0});
            menu->ignoreAnchorPointForPosition(false);
            menu->setAnchorPoint(ccp(0, 0));

            std::stringstream fntName;
            fntName << "Font ";
            fntName << i + 1;

            std::stringstream fntFile;

            if (i == 0) {
                fntFile << "bigFont.fnt";
            }
            else if (i < 10) {
                fntFile << "gjFont0";
                fntFile << i;
                fntFile << ".fnt";
            }
            else {
                fntFile << "gjFont";
                fntFile << i;
                fntFile << ".fnt";
            }

            auto font = CCLabelBMFont::create(fntName.str().c_str(), fntFile.str().c_str());
            font->setAnchorPoint(ccp(0, 0.5f));
            font->setScale(0.65f);
            font->setPosition(ccp(2, 15));
            menu->addChild(font);

            auto btn = CCMenuItemToggler::create(unSel, sel, menu, menu_selector(BetterSelectFontLayer::onSelect));
            btn->setTag(i);
            btn->setPosition(node->getContentSize().width - 20, 15);
            btn->toggle(static_cast<int>(i) == p0->m_levelSettings->m_fontIndex);
            btn->setEnabled(!(static_cast<int>(i) == p0->m_levelSettings->m_fontIndex));

            menu->addChild(btn);

            SelFont::buttons.push_back(btn);

            node->addChild(menu);
            content->addChild(node);
        }

        scroll->moveToTop();

        this->addChild(scroll);

        handleTouchPriority(this);

        return true;
    }
};
