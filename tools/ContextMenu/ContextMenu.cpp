#include "ContextMenu.hpp"

using namespace gd;
using namespace gdmake;
using namespace gdmake::extra;
using namespace cocos2d;

void ccDrawColor4B(ccColor4B const& c) {
    ccDrawColor4B(c.r, c.g, c.b, c.a);
}

ccColor4F to4f(ccColor4B const& c) {
    return { c.r / 255.f, c.g / 255.f, c.b / 255.f, c.a / 255.f };
}

ccColor3B to3B(ccColor4B const& c) {
    return { c.r, c.g, c.b };
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
        to4f(this->m_bSuperMouseHovered ? g_colHover : g_colTransparent)
    );
}

bool ContextMenuItem::mouseDownSuper(MouseButton btn, CCPoint const& mpos) {
    if (btn == kMouseButtonLeft) {
        this->activate();
        this->m_obLastMousePos = mpos;
        SuperMouseManager::get()->captureMouse(this);
    }
    return true;
}

bool ContextMenuItem::mouseUpSuper(MouseButton btn, CCPoint const&) {
    if (btn == kMouseButtonLeft) {
        SuperMouseManager::get()->releaseCapture(this);
    }
    return true;
}

void ContextMenuItem::mouseMoveSuper(CCPoint const& mpos) {
    if (this->m_bSuperMouseDown) {
        this->drag(mpos.y - this->m_obLastMousePos.y);
        this->m_obLastMousePos = mpos;
    }
}

void ContextMenuItem::activate() {}
void ContextMenuItem::drag(float) {}


bool KeybindContextMenuItem::init(keybind_id const& id) {
    if (!ContextMenuItem::init())
        return false;
    
    this->m_sID = id;

    this->m_pLabel = CCLabelBMFont::create("", "chatFont.fnt");
    this->m_pLabel->setColor(to3B(g_colText));
    this->addChild(this->m_pLabel);

    auto target = KeybindManager::get()->getTargetByID(id);

    if (target.bind)
        this->m_pLabel->setString(target.bind->name.c_str());
    else
        this->m_pLabel->setString("Invalid\nkeybind");

    return true;
}

void KeybindContextMenuItem::visit() {
    ContextMenuItem::visit();

    this->m_pLabel->setPosition(
        this->getContentSize() / 2
    );
    this->m_pLabel->limitLabelWidth(
        this->getScaledContentSize().width -10.0f, .5f, .0f
    );
    this->setSuperMouseHitOffset(this->getScaledContentSize() / 2);
}

void KeybindContextMenuItem::activate() {

}

KeybindContextMenuItem* KeybindContextMenuItem::create(keybind_id const& id) {
    auto ret = new KeybindContextMenuItem;

    if (ret && ret->init(id)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}


bool PropContextMenuItem::init(PropContextMenuItem::Type type) {
    if (!ContextMenuItem::init())
        return false;
    
    this->m_eType = type;

    return true;
}

void PropContextMenuItem::drag(float val) {
    auto objs = LevelEditorLayer::get()->getEditorUI()->getSelectedObjects();
    CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject) {
        switch (this->m_eType) {
            case kTypeScale: 
                obj->setScale(obj->getScale() + val / 10.f);
                break;
            case kTypeRotate: 
                obj->setScale(obj->getScale() + val / 10.f);
                break;
            case kTypeZOrder: 
                obj->setScale(obj->getScale() + val / 10.f);
                break;
            case kTypeELayer: 
                m_fDragCollect += val / 2.f;
                while (m_fDragCollect > 1.f || m_fDragCollect < -1.f) {
                    m_fDragCollect += m_fDragCollect > 0 ? -1.f : 1.f;
                    obj->m_nEditorLayer += m_fDragCollect > 0 ? 1 : -1;
                }
                break;
            case kTypeELayer2: 
                m_fDragCollect += val / 2.f;
                while (m_fDragCollect > 1.f || m_fDragCollect < -1.f) {
                    m_fDragCollect += m_fDragCollect > 0 ? -1.f : 1.f;
                    obj->m_nEditorLayer2 += m_fDragCollect > 0 ? 1 : -1;
                }
                break;
        }
    }
}

PropContextMenuItem* PropContextMenuItem::create(PropContextMenuItem::Type type) {
    auto ret = new PropContextMenuItem;

    if (ret && ret->init(type)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}


bool ContextMenu::init() {
    if (!CCLayerColor::initWithColor(g_colBG, 100.0f, 60.0f))
        return false;
    
    this->setTag(CMENU_TAG);
    this->setZOrder(5000);
    this->setSuperMouseHitOffset(this->getScaledContentSize() / 2);
    this->m_pEditor = LevelEditorLayer::get();

    this->m_mConfig = {
        { kStateNoneSelected, {
            { "gd.edit.paste", "betteredit.select_all" },
            { "betteredit.toggle_ui" }
        } },

        { kStateOneSelected, {
            { "betteredit.preview_mode" }
        } }
    };

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
    this->generate();
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
    return this->isVisible();
}

void ContextMenu::generate() {
    this->removeAllChildren();

    auto sel = this->m_pEditor->getEditorUI()->getSelectedObjects();

    Config c;

    if (!sel->count()) {
        c = this->m_mConfig[kStateNoneSelected];
    } else if (sel->count() == 1) {
        c = this->m_mConfig[kStateOneSelected];
    } else {
        c = this->m_mConfig[kStateManySelected];
    }

    auto h = c.size() * 18.0f;
    auto w = 140.0f;

    float y = 0.0f;
    for (auto const& line : c) {
        float x = 0.0f;
        for (auto const& item : line) {
            ContextMenuItem* pItem = nullptr;
            switch (item.m_eType) {
                case ContextMenuStorageItem::kItemTypeKeybind:
                    pItem = KeybindContextMenuItem::create(item.m_sKeybindID);
                    break;
                case ContextMenuStorageItem::kItemTypeProperty:
                    pItem = PropContextMenuItem::create(item.m_ePropType);
                    break;
            }
            if (pItem) {
                pItem->setPosition(x, y);
                pItem->setContentSize({w / line.size(), 25.f});
                this->addChild(pItem);
            }
            x += w / line.size();
        }
        y += 25.f;
    }

    this->setContentSize({ w, h });
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
