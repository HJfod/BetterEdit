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

ccColor4B g_colBG = { 0, 0, 0, 200 };
ccColor4B g_colText = { 255, 255, 255, 255 };
ccColor4B g_colGray = { 150, 150, 150, 255 };
ccColor4B g_colHover = { 255, 255, 255, 50 };
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

    this->setSuperMouseHitSize(this->getScaledContentSize());
    this->setSuperMouseHitOffset(this->getScaledContentSize() / 2);
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

    this->m_pLabel = CCLabelTTF::create("", "Segoe UI", 16);
    this->m_pLabel->setColor(to3B(g_colText));
    this->addChild(this->m_pLabel);

    auto target = KeybindManager::get()->getTargetByID(id);

    this->m_pCallback = target.bind;

    if (target.bind)
        this->m_pLabel->setString(target.bind->name.c_str());
    else
        this->m_pLabel->setString("Invalid\nkeybind");

    return true;
}

void KeybindContextMenuItem::visit() {
    this->m_pLabel->setPosition(
        this->getContentSize() / 2
    );
    this->m_pLabel->setScale(.3f);
    this->m_pLabel->setScale(min(
        1.0f,
        (this->getScaledContentSize().width - 10.0f) /
        this->m_pLabel->getScaledContentSize().width
    ) * .3f);
    // this->m_pLabel->limitLabelWidth(
    //     this->getScaledContentSize().width -10.0f, .4f, .0f
    // );

    ContextMenuItem::visit();
}

void KeybindContextMenuItem::activate() {
    KeybindManager::get()->invokeCallback(
        this->m_sID, LevelEditorLayer::get()->getEditorUI(), nullptr
    );
    SuperMouseManager::get()->releaseCapture(this);
    if (this->m_pMenu)
        this->m_pMenu->hide();
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


bool SpecialContextMenuItem::init(
    const char* spr, const char* txt, SpecialContextMenuItem::Callback cb
) {
    if (!ContextMenuItem::init())
        return false;
    
    this->m_pSprite = CCSprite::createWithSpriteFrameName(spr);
    this->addChild(this->m_pSprite);

    this->m_pLabel = CCLabelTTF::create(txt, "Segoe UI", 16);
    this->m_pLabel->setColor(to3B(g_colText));
    this->addChild(this->m_pLabel);

    this->m_pCallback = cb;

    return true;
}

void SpecialContextMenuItem::visit() {
    this->m_pLabel->setScale(.3f);
    this->m_pLabel->setPosition({
        this->getContentSize().width / 2 + 20.0f,
        this->getContentSize().height / 2
    });

    this->m_pSprite->setPosition({
        this->getContentSize().width / 2 - 
            this->m_pLabel->getScaledContentSize().width / 2,
        this->getContentSize().height / 2
    });
    this->m_pSprite->setScale(.2f);

    ContextMenuItem::visit();
}

void SpecialContextMenuItem::activate() {
    this->m_pCallback(this);
    SuperMouseManager::get()->releaseCapture(this);
    if (this->m_pMenu)
        this->m_pMenu->hide();
}

SpecialContextMenuItem* SpecialContextMenuItem::create(
    const char* spr, const char* txt, SpecialContextMenuItem::Callback cb
) {
    auto ret = new SpecialContextMenuItem;

    if (ret && ret->init(spr, txt, cb)) {
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
    this->m_pEditor = LevelEditorLayer::get();

    this->m_mConfig = {
        { kStateNoneSelected, {
            { 
              "betteredit.select_all_left",
              "betteredit.select_all",
              "betteredit.select_all_right",
            },
            { "gd.edit.paste", "betteredit.toggle_ui" },
        } },

        { kStateOneSelected, {
            { "betteredit.preview_mode" },
            { "gd.edit.deselect" },
        } },

        { kStateManySelected, {
            { "betteredit.edit_object", "betteredit.edit_group", "betteredit.edit_special" },
            { "betteredit.align_x", "betteredit.align_y" },
            { "gd.edit.deselect" },
        } },
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
    this->setSuperMouseHitOffset(this->getScaledContentSize() / 2);
    this->setSuperMouseHitSize(this->getScaledContentSize());
    this->setVisible(true);
}

void ContextMenu::hide() {
    this->setVisible(false);
    this->setPosition(-700.0f, -900.0f);
}

void ContextMenu::mouseDownOutsideSuper(MouseButton, CCPoint const&) {
    this->hide();
}

bool ContextMenu::mouseDownSuper(MouseButton, CCPoint const&) {
    return false;
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

    static constexpr const float defaultItemHeight = 12.f;

    auto h = c.size() * defaultItemHeight + defaultItemHeight;
    auto w = 120.0f;

    float y = (c.size() - 1) * defaultItemHeight + defaultItemHeight;
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
                pItem->setContentSize({w / line.size(), defaultItemHeight});
                pItem->m_pMenu = this;
                this->addChild(pItem);
            }
            x += w / line.size();
        }
        y -= defaultItemHeight;
    }

    auto settings = SpecialContextMenuItem::create(
        "GJ_optionsBtn02_001.png", "Customize", [](auto btn) -> void {}
    );
    settings->setPosition(0, y);
    settings->setContentSize({ w, defaultItemHeight });
    settings->m_pMenu = this;
    this->addChild(settings);

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
