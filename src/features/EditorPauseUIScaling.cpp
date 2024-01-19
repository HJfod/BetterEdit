#include <Geode/Bindings.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>

using namespace geode::prelude;

class $modify (EditorPauseLayer)
{
    bool init(LevelEditorLayer* p0)
    {
        if (!EditorPauseLayer::init(p0))
            return false;

        if (!Mod::get()->getSettingValue<bool>("scale-pause"))
            return true;

        float scale = Mod::get()->getSettingValue<double>("scale-factor");
        auto size = CCDirector::get()->getWinSize();

        auto mainButtons = reinterpret_cast<CCMenu*>(this->getChildren()->objectAtIndex(0));
        mainButtons->setContentSize(ccp(0, 0));
        mainButtons->ignoreAnchorPointForPosition(false);
        mainButtons->setScale(scale);

        auto objCount = reinterpret_cast<CCLabelBMFont*>(this->getChildren()->objectAtIndex(13));
        objCount->setPosition(ccp(objCount->getPositionX() * scale, size.height - ((size.height - objCount->getPositionY()) * scale) ));
        objCount->setScale(0.5f * scale);

        auto length = reinterpret_cast<CCLabelBMFont*>(this->getChildren()->objectAtIndex(14));
        length->setPosition(ccp(length->getPositionX() * scale, size.height - ((size.height - length->getPositionY()) * scale) ));
        length->setScale(0.5f * scale);

        auto buttons = reinterpret_cast<CCMenu*>(this->getChildren()->objectAtIndex(1));

        auto lines = reinterpret_cast<CCMenuItemSpriteExtra*>(buttons->getChildren()->objectAtIndex(0));
        lines->setScale(scale);
        lines->m_baseScale = scale;
        lines->setPosition(lines->getPosition() * scale);
        
        auto linesOff = reinterpret_cast<CCMenuItemSpriteExtra*>(buttons->getChildren()->objectAtIndex(2));
        linesOff->setScale(scale);
        linesOff->m_baseScale = scale;
        linesOff->setPosition(linesOff->getPosition() * scale);

        auto bpm = reinterpret_cast<CCMenuItemSpriteExtra*>(buttons->getChildren()->objectAtIndex(1));
        bpm->setScale(scale);
        bpm->m_baseScale = scale;
        bpm->setPosition(bpm->getPosition() * scale);

        auto toggleMenu = CCMenu::create();
        toggleMenu->setID("toggle-menu");
        toggleMenu->setContentSize(ccp(0, 0));
        toggleMenu->setPosition(ccp(0, 0));
        toggleMenu->setScale(scale);

        std::vector<CCMenuItemToggler*> vec = {};

        //replace 11 with the toggler count
        for (size_t i = 0; i < 11; i++)
        {
            int v = 19 + i;

            auto btn = reinterpret_cast<CCMenuItemToggler*>(buttons->getChildren()->objectAtIndex(v));

            vec.push_back(btn);

            auto text = reinterpret_cast<CCLabelBMFont*>(this->getChildren()->objectAtIndex(2 + i));
            text->setPosition(text->getPosition() * scale);
            text->setScale(text->getScale() * scale);
        }

        for (size_t i = 0; i < vec.size(); i++)
        {
            vec[i]->removeFromParentAndCleanup(false);
            vec[i]->setPosition(ccp(((-1 * size.width / 2) - vec[i]->getPositionX()) * -1, 24 * (i + 1)));
            toggleMenu->addChild(vec[i]);
        }

        for (size_t i = 3; i < buttons->getChildrenCount(); i++)
        {
            auto b = reinterpret_cast<CCMenuItemSpriteExtra*>(buttons->getChildren()->objectAtIndex(i));

            b->setPosition(ccp(size.width / 2 - (((size.width / 2) - b->getPositionX()) * scale), (b->getPositionY() * scale)));
            b->m_baseScale = scale;
            b->setScale(scale);
        }

        buttons->setPositionY(buttons->getPositionY() * scale);

        this->addChild(toggleMenu);

        return true;
    }
};