#include <Geode/cocos/label_nodes/CCLabelBMFont.h>
#include <Geode/Geode.hpp>
#include <Geode/modify/Modify.hpp>
#include <Geode/modify/CCMenuItemSpriteExtra.hpp>
#include <Geode/cocos/cocoa/CCObject.h>
#include <Geode/modify/GJGameLevel.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <string>

using namespace geode::prelude;

struct NextFreeLayer : Modify<NextFreeLayer, EditorUI> {
  bool init(LevelEditorLayer* editorLayer) {
    if (!EditorUI::init(editorLayer)) {
      return false;
    }

    auto layers_menu = this->getChildByIDRecursive("layer-menu");
    auto next_free_layer_spr = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png"_spr);
  
    next_free_layer_spr->setScale(0.54f);
    next_free_layer_spr->setOpacity(175);

    auto next_free_layer_btn = CCMenuItemSpriteExtra::create(
      next_free_layer_spr,
      this,
      menu_selector(NextFreeLayer::onNextFree)
    );

    layers_menu->addChild(next_free_layer_btn);

    return true;
  }

  void onNextFree(CCObject* pSender) {
    cocos2d::CCArray *objects = this->m_editorLayer->m_objects;
    short current_layer = this->m_editorLayer->m_currentLayer;
    int next_free_layer = this->m_editorLayer->getNextFreeEditorLayer(objects);

    this->m_editorLayer->m_currentLayer = next_free_layer;
    CCLabelBMFont* layer_label = this->m_currentLayerLabel;

    layer_label->setString(
        std::to_string(next_free_layer).c_str()
    );
  }
};
