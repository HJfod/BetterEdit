
#include <Geode/Bindings.hpp>
#include <Geode/modify/SelectFontLayer.hpp>
#include <Geode/Loader.hpp>

using namespace geode::prelude;

class SelFont
{
    public:
        //this shits not working in fields :(
        static inline std::vector<CCMenuItemToggler*> buttons = {};
};

class $modify (BetterSelectFontLayer, SelectFontLayer)
{
    void onSelect(CCObject* sender)
    {
        int s = reinterpret_cast<CCNode*>(sender)->getTag();

        GameManager::sharedState()->m_levelEditorLayer->updateLevelFont(s);

        for (size_t i = 0; i < SelFont::buttons.size(); i++)
        {
            if (i != s)
            {
                SelFont::buttons[i]->toggle(false);    
                SelFont::buttons[i]->setEnabled(true);
            }
            else
            {
                SelFont::buttons[i]->toggle(true);
                SelFont::buttons[i]->setEnabled(false);
            }
        }
    }

    bool init(LevelEditorLayer* p0)
    {
        if (!SelectFontLayer::init(p0))
            return false;

        //position old stuff and hide

        reinterpret_cast<CCScale9Sprite*>(reinterpret_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0))->setContentSize(ccp(400, 290));
        reinterpret_cast<CCLayer*>(this->getChildren()->objectAtIndex(1))->setPositionY(290);
        reinterpret_cast<CCLayer*>(this->getChildren()->objectAtIndex(2))->setVisible(false);

        auto butMenu = reinterpret_cast<CCMenu*>(reinterpret_cast<CCLayer*>(this->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(1));
        reinterpret_cast<CCNode*>(butMenu->getChildren()->objectAtIndex(0))->setPositionY(-45);
        reinterpret_cast<CCNode*>(butMenu->getChildren()->objectAtIndex(1))->setPosition(ccp(-182, 200));
        reinterpret_cast<CCNode*>(butMenu->getChildren()->objectAtIndex(2))->setVisible(false);
        reinterpret_cast<CCNode*>(butMenu->getChildren()->objectAtIndex(3))->setVisible(false);

        auto scroll = ScrollLayer::create(ccp(320, 210));
        scroll->setAnchorPoint(ccp(0, 0));
        scroll->setPosition(CCDirector::get()->getWinSize() / 2 - scroll->getContentSize() / 2 + ccp(0, 10));

        auto content = scroll->m_contentLayer;

        int fontCount = 60;
        content->setContentSize(ccp(320, 30 * (fontCount)));

        int selectedFont = p0->m_levelSettings->m_fontIndex;

        auto unSel = CCSprite::createWithSpriteFrameName("GJ_selectSongBtn_001.png");
        auto sel = CCSprite::createWithSpriteFrameName("GJ_selectSongOnBtn_001.png");
        unSel->setScale(0.6f);
        sel->setScale(0.6f);

        SelFont::buttons = {};

        for (size_t i = 0; i < fontCount; i++)
        {
            auto node = CCLayerColor::create();
            node->setColor({0, 0, 0});
            node->setOpacity((i % 2 == 0) ? 100 : 50);
            node->setContentSize(ccp(320, 30));
            node->setPosition(ccp(0, 30 * fontCount - ((i + 1) * 30)));

            auto menu = CCMenu::create();
            menu->setPosition(ccp(0, 0));
            menu->setContentSize({0, 0});
            menu->ignoreAnchorPointForPosition(false);
            menu->setAnchorPoint(ccp(0, 0));

            std::stringstream fntName;
            fntName << "Font ";
            fntName << i + 1;

            std::stringstream fntFile;

            if (i == 0)
            {
                fntFile << "bigFont.fnt";
            }
            else if (i < 10)
            {
                fntFile << "gjFont0";
                fntFile << i;
                fntFile << ".fnt";
            }
            else
            {
                fntFile << "gjFont";
                fntFile << i;
                fntFile << ".fnt";
            }

            log::info(fntFile.str());

            auto font = CCLabelBMFont::create(fntName.str().c_str(), fntFile.str().c_str());
            font->setAnchorPoint(ccp(0, 0.5f));
            font->setScale(0.65f);
            font->setPosition(ccp(2, 15));
            menu->addChild(font);

            auto btn = CCMenuItemToggler::create(unSel, sel, menu, menu_selector(BetterSelectFontLayer::onSelect));
            btn->setTag(i);
            btn->setPosition(node->getContentSize().width - 20, 15);
            btn->toggle(i == p0->m_levelSettings->m_fontIndex);
            btn->setEnabled(!(i == p0->m_levelSettings->m_fontIndex));

            menu->addChild(btn);

            SelFont::buttons.push_back(btn);

            node->addChild(menu);
            content->addChild(node);
        }

        scroll->moveToTop();

        this->addChild(scroll);

        return true;
    }
};