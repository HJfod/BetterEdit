#include "ContextMenu.hpp"

using namespace gd;
using namespace gdmake;
using namespace gdmake::extra;
using namespace cocos2d;

constexpr const int CMENU_TAG = 600;

bool ContextMenu::init() {
    if (!CCLayerColor::initWithColor(this->m_colBG, 100.0f, 60.0f))
        return false;
    
    this->setTag(CMENU_TAG);

    return true;
}

void ContextMenu::show(CCPoint const& pos) {
    this->setPosition(pos + this->getScaledContentSize() / 2);
    this->setVisible(true);
}

void ContextMenu::hide() {
    this->setVisible(false);
}

void ContextMenu::mouseDownOutsideSuper(MouseButton, CCPoint const&) {
    this->hide();
}

ContextMenu* ContextMenu::get() {
    if (!GameManager::sharedState()->getEditorLayer())
        return nullptr;
    
    return as<ContextMenu*>(GameManager::sharedState()
        ->getEditorLayer()
        ->getEditorUI()
        ->getChildByTag(CMENU_TAG));
}

ContextMenu* ContextMenu::create() {
    auto pRet = new ContextMenu();

    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
