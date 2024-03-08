#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "ScriptSelectLayer.hpp"
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/GameObject.hpp>
#include <geode.custom-keybinds/include/Keybinds.hpp>
#include "LuaManager.hpp"

#include <utils/HandleUIHide.hpp>



using namespace cocos2d;
using namespace geode;
using namespace keybinds;

struct MyEditorUI : geode::Modify<MyEditorUI, EditorUI>
{
    void onBtn(CCObject*)
    {
        ScriptSelectLayer::create()->show();
    }

    void setBindings()
    {
        ScriptInfo::forAllOnPath([&](const ScriptInfo& script){
            this->template addEventListener<InvokeBindFilter>([script /*copy*/](InvokeBindEvent* event){
                if (event->isDown())
                {
                    LuaManager::runScript(script);
                }
                return ListenerResult::Propagate;
            }, script.getUniqueString());
        });
    }
    bool init(LevelEditorLayer* editor)
    {
        if(!EditorUI::init(editor)) return false;
        auto spr = CCSprite::createWithSpriteFrameName("be-button.png"_spr);
        spr->setScale(.75f);
        auto btn = CCMenuItemSpriteExtra::create(spr, nullptr, this, menu_selector(MyEditorUI::onBtn));
        handleUIHideOnPlaytest(this, btn);
        auto menu = static_cast<CCMenu*>(this->getChildByID("undo-menu"));
        menu->addChild(btn);
        menu->updateLayout();
        setBindings();
        return true;
    }
};

//struct MyMenuLayer : geode::Modify<MyMenuLayer, MenuLayer>
//{
//    void onMoreGames(CCObject*)
//    {
//        ScriptSelectLayer::create()->show();
//    }
//};

$execute
{
    ScriptSelectLayer::updateBindings();
}