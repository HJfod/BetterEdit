#include "ContextMenu.hpp"

using namespace gd;
using namespace gdmake;
using namespace gdmake::extra;
using namespace cocos2d;

void ccDrawColor4B(ccColor4B const& c) {
    ccDrawColor4B(c.r, c.g, c.b, c.a);
}

constexpr const int CMENU_TAG = 600;

ccColor4B g_colBG = { 255, 255, 255, 255 };
ccColor4B g_colText = { 0, 0, 0, 255 };
ccColor4B g_colGray = { 150, 150, 150, 255 };
ccColor4B g_colHover = { 0, 0, 0, 50 };
ccColor4B g_colTransparent = { 0, 0, 0, 0 };

bool ContextMenuItem::init() {
    if (!CCNode::init())
        return false;
    
    return true;
}

void ContextMenuItem::draw() {
    CCNode::draw();

    ccDrawSolidRect(
        { 0, 0 },
        this->getScaledContentSize(),
        this->m_bSuperMouseHovered ? g_colHover : g_colTransparent
    );
}

void ContextMenuItem::mouseDownSuper(MouseButton btn, CCPoint const& mpos) override {
    if (btn == kMouseButtonLeft) {
        this->activate();
        this->m_obLastMousePos = mpos;
        SuperMouseManager::get()->captureMouse(this);
    }
}

void ContextMenuItem::mouseUpSuper(MouseButton btn, CCPoint const&) override {
    if (btn == kMouseButtonLeft) {
        SuperMouseManager::get()->releaseCapture(this);
    }
}

void ContextMenuItem::mouseMoveSuper(CCPoint const& mpos) override {
    if (this->m_bSuperMouseDown) {
        this->drag(mpos.y - this->m_obLastMousePos.y);
        this->m_obLastMousePos = mpos;
    }
}

void ContextMenuItem::activate() {}
void ContextMenuItem::drag(int) {}




bool ContextMenu::init() {
    if (!CCLayerColor::initWithColor(g_colBG, 100.0f, 60.0f))
        return false;
    
    this->setTag(CMENU_TAG);
    this->setZOrder(5000);
    this->setSuperMouseHitOffset(this->getScaledContentSize() / 2);

    return true;
}

void ContextMenu::draw() {
    CCLayerColor::draw();

    if (m_bDrawBorder) {
        ccDrawColor4B(g_colText);
        ccDrawRect(
            { 0, 0 },
            this->getScaledContentSize()
        );
    }
}

void ContextMenu::show() {
    this->show(getMousePos());
}

void ContextMenu::show(CCPoint const& pos) {
    this->setPosition(
        pos.x,
        pos.y - this->getScaledContentSize().height
    );
    this->setVisible(true);
}

void ContextMenu::hide() {
    this->setVisible(false);
}

void ContextMenu::mouseDownOutsideSuper(MouseButton, CCPoint const&) {
    this->hide();
}

bool ContextMenu::mouseDownSuper(MouseButton, CCPoint const&) {
    return true;
}

ContextMenu* ContextMenu::get() {
    if (!GameManager::sharedState()->getEditorLayer())
        return nullptr;
    
    return as<ContextMenu*>(GameManager::sharedState()
        ->getEditorLayer()
        ->getChildByTag(CMENU_TAG));
}

ContextMenu* ContextMenu::load() {
    auto m = ContextMenu::create();

    LevelEditorLayer::get()
        ->addChild(m);
    
    m->hide();
    
    return m;
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
