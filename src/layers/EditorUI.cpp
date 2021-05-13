#include "EditorUI.hpp"
#include "ToolsLayer.hpp"
#include "../nodes/InputNode.hpp"

void EditorUI::onShowTools(cocos2d::CCObject* pSender) {
    ToolsLayer::create()->show();
}

bool __fastcall EditorUI::initHook(EditorUI* _ui, uintptr_t, uintptr_t _gm) {
    bool ret = EditorUI::init(_ui, _gm);

    auto menu = getChild<cocos2d::CCMenu*>(_ui, 2);
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    auto remapSpr = cocos2d::CCSprite::createWithSpriteFrameName("communityIcon_03_001.png");
    remapSpr->setScale(.75f);

    auto remapBtn = gd::CCMenuItemSpriteExtra::create(
        remapSpr,
        menu,
        (cocos2d::SEL_MenuHandler)&EditorUI::onShowTools
    );

    auto delButtonPos = getChild<gd::CCMenuItemSpriteExtra*>(menu, 9)->getPosition();

    remapBtn->setPosition(delButtonPos.x + 50.0f, delButtonPos.y);
    menu->addChild(remapBtn);

    auto node = InputNode::create(120.0f, "ffs");
    gd::GameManager::sharedState()->getEditorLayer()->addChild(node);

    return ret;
}

