#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "ScriptSelectLayer.hpp"
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/GameObject.hpp>


using namespace cocos2d;

struct MyEditorUI : geode::Modify<MyEditorUI, EditorUI>
{
    void onBtn(CCObject*)
    {
        ScriptSelectLayer::create()->show();
    }
    bool init(LevelEditorLayer* editor)
    {
        if(!EditorUI::init(editor)) return false;
        auto spr = CCSprite::createWithSpriteFrameName("GJ_newBtn_001.png");
        spr->setScale(.5f);
        auto btn = CCMenuItemSpriteExtra::create(spr, nullptr, this, menu_selector(MyEditorUI::onBtn));
        auto menu = static_cast<CCMenu*>(this->getChildByID("undo-menu"));
        menu->addChild(btn);
        menu->updateLayout();
        return true;
    }
};

struct MyMenuLayer : geode::Modify<MyMenuLayer, MenuLayer>
{
    void onMoreGames(CCObject*)
    {
        ScriptSelectLayer::create()->show();
    }
};