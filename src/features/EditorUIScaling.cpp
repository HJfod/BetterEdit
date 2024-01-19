#include <Geode/Bindings.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify (EditorUI)
{
    bool init(LevelEditorLayer* p0)
    {
        if (!EditorUI::init(p0))
            return false;

        float scale = Mod::get()->getSettingValue<double>("scale-factor");
        auto size = CCDirector::get()->getWinSize();

        auto slider = reinterpret_cast<Slider*>(this->getChildren()->objectAtIndex(0));
        slider->ignoreAnchorPointForPosition(false);
        slider->setContentSize(ccp(0, 0));
        slider->setScale(scale);
        slider->setPositionY(size.height - (size.height - slider->getPositionY()) * scale);

        
        auto sliderO = this->getChildByID("position-slider");
        
        sliderO->setPositionY(size.height - (size.height - sliderO->getPositionY()) * scale);
        sliderO->setContentSize(ccp(0, 0));
        sliderO->setScale(scale);

        auto settingsMenu = this->getChildByID("settings-menu");
        
        settingsMenu->setPosition(size);
        settingsMenu->setAnchorPoint(ccp(1, 1));
        settingsMenu->setScale(scale);

        auto buttonsMenu = this->getChildByID("editor-buttons-menu");
        
        buttonsMenu->setPositionX((buttonsMenu->getPositionX() + buttonsMenu->getContentSize().width / 2));
        buttonsMenu->setAnchorPoint(ccp(1, 0.5f));
        buttonsMenu->setScale(scale);

        auto layerMenu = this->getChildByID("layer-menu");
        
        layerMenu->setPositionX(layerMenu->getPositionX() + layerMenu->getContentSize().width / 2);
        layerMenu->setPositionY(buttonsMenu->getPositionY() - (buttonsMenu->getPositionY() - layerMenu->getPositionY()) * scale);
        layerMenu->setAnchorPoint(ccp(1, 0.5f));
        layerMenu->setScale(scale);

        auto undoMenu = this->getChildByID("undo-menu");
        undoMenu->setPosition(ccp((undoMenu->getPositionX() - undoMenu->getContentSize().width / 2) * scale, size.height));
        undoMenu->setAnchorPoint(ccp(0, 1));
        undoMenu->setScale(scale);

        auto testMenu = this->getChildByID("playtest-menu");
        testMenu->setPosition(ccp((testMenu->getPositionX() - testMenu->getContentSize().width / 2) * scale, testMenu->getPositionY()));
        testMenu->setAnchorPoint(ccp(0, 0.5f));
        testMenu->setScale(scale);

        auto playMenu = this->getChildByID("playback-menu");
        playMenu->setPosition(ccp((playMenu->getPositionX() - playMenu->getContentSize().width / 2) * scale, testMenu->getPositionY() + (playMenu->getPositionY() - testMenu->getPositionY()) * scale));
        playMenu->setAnchorPoint(ccp(0, 0.5f));
        playMenu->setScale(scale);

        auto zoomMenu = this->getChildByID("zoom-menu");
        zoomMenu->setPosition(ccp((zoomMenu->getPositionX() - zoomMenu->getContentSize().width / 2) * scale, testMenu->getPositionY() + (zoomMenu->getPositionY() - testMenu->getPositionY()) * scale));
        zoomMenu->setAnchorPoint(ccp(0, 0.5f));
        zoomMenu->setScale(scale);

        auto linkMenu = this->getChildByID("link-menu");
        linkMenu->setPosition(ccp((linkMenu->getPositionX() - linkMenu->getContentSize().width / 2) * scale, testMenu->getPositionY() + (linkMenu->getPositionY() - testMenu->getPositionY()) * scale));
        linkMenu->setAnchorPoint(ccp(0, 0.5f));
        linkMenu->setScale(scale);

        // i am very sorry for using object indexes but no id

        auto objBG = static_cast<CCSprite*>(this->getChildren()->objectAtIndex(1));
        objBG->setScaleY(scale);

        auto objTabs = this->getChildByID("build-tabs-menu");
        objTabs->setAnchorPoint(ccp(0.5f, 0));
        objTabs->setPositionY(objTabs->getPositionY() - objTabs->getContentSize().height / 2);
        objTabs->setPositionY(objTabs->getPositionY() * scale);
        objTabs->setPositionY(objTabs->getPositionY() - 1);
        
        if (Mod::get()->getSettingValue<bool>("scale-build-tabs"))
            objTabs->setScale(scale);

        auto leftTabs = this->getChildByID("toolbar-categories-menu");
        leftTabs->setAnchorPoint(ccp(0, 0));
        leftTabs->setPosition(ccp(leftTabs->getPositionX() - leftTabs->getContentSize().width / 2, 0));
        leftTabs->setScale(scale);

        auto rightTabs = this->getChildByID("toolbar-toggles-menu");
        rightTabs->setAnchorPoint(ccp(1, 0));
        rightTabs->setPosition(ccp(rightTabs->getPositionX() + rightTabs->getContentSize().width / 2, 0));
        rightTabs->setScale(scale);

        auto leftLine = static_cast<CCSprite*>(this->getChildren()->objectAtIndex(4));
        leftLine->setScale(scale);
        leftLine->setPosition(leftLine->getPosition() * scale);

        auto rightLine = static_cast<CCSprite*>(this->getChildren()->objectAtIndex(5));
        rightLine->setScale(scale);
        rightLine->setPosition(ccp(size.width - (size.width - rightLine->getPositionX()) * scale, rightLine->getPositionY() * scale));

        auto deleteTabs = this->getChildByID("delete-category-menu");
        deleteTabs->setContentSize(ccp(0, 0));
        deleteTabs->setPositionY(deleteTabs->getPositionY() * scale);
        deleteTabs->setScale(scale);

        std::vector<EditButtonBar*> bars = {};
        auto barMenu = CCMenu::create();
        barMenu->setAnchorPoint(ccp(0.5f, 0));
        barMenu->setPosition(ccp(0, 0));
        barMenu->setScale(scale);
        barMenu->setID("object-buttons");

        for (size_t i = 0; i < this->getChildrenCount(); i++)
        {
            if (typeinfo_cast<EditButtonBar*>(this->getChildren()->objectAtIndex(i)))
                bars.push_back(static_cast<EditButtonBar*>(this->getChildren()->objectAtIndex(i)));
        }
        
        for (size_t i = 0; i < bars.size(); i++)
        {
            bars[i]->removeFromParentAndCleanup(false);            
            barMenu->addChild(bars[i]);
        }

        this->addChild(barMenu, 1);

        return true;
    }
};