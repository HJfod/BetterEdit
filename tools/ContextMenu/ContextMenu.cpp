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

void limitSpriteSize(CCSprite* spr, CCSize const& size, float def, float min) {
    spr->setScale(1.f);
    auto [cwidth, cheight] = spr->getContentSize();
    
    auto len = size.height;
    auto clen = cheight;
    if (size.width < size.height) {
        len = size.width;
        clen = cwidth;
    }

    float scale = def;
    if (len && len < clen) {
        scale = len / clen;
    }
    if (def && def < scale) {
        scale = def;
    }
    if (min && scale < min) {
        scale = min;
    }
    spr->setScale(scale);
}

CCSize operator-(CCSize const& size, float f) {
    return { size.width - f, size.height - f };
}

constexpr const int CMENU_TAG = 600;

#define MORD(x, d) (this->m_pMenu ? this->m_pMenu->x : d)

ccColor4B g_colBG = { 0, 0, 0, 200 };
ccColor4B g_colText = { 255, 255, 255, 255 };
ccColor4B g_colGray = { 150, 150, 150, 255 };
ccColor4B g_colHover = { 255, 255, 255, 52 };
ccColor4B g_colTransparent = { 0, 0, 0, 0 };

bool ContextMenuItem::init(ContextMenu* menu) {
    if (!CCNode::init())
        return false;
    
    this->m_pMenu = menu;
    
    return true;
}

void ContextMenuItem::draw() {
    CCNode::draw();

    ccColor4B col = g_colHover;

    if (this->m_bSuperMouseHovered) {
        if (this->m_nFade < g_colHover.a)
            this->m_nFade += MORD(m_nAnimationSpeed, 5);
        else
            this->m_nFade = g_colHover.a;
    } else {
        if (this->m_nFade >= MORD(m_nAnimationSpeed, 5))
            this->m_nFade -= MORD(m_nAnimationSpeed, 5);
        else
            this->m_nFade = 0;
    }

    col.a = this->m_nFade;

    ccDrawSolidRect(
        { 0, 0 },
        this->getScaledContentSize(),
        to4f(col)
    );

    auto mul = this->m_pMenu ? this->m_pMenu->getScale() : 1.f;

    this->setSuperMouseHitSize(this->getScaledContentSize() * mul);
    this->setSuperMouseHitOffset(this->getScaledContentSize() / 2 * mul);
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

AnyLabel* ContextMenuItem::createLabel(const char* txt) {
    if (this->m_pMenu) {
        switch (this->m_pMenu->m_eType) {
            case kAnyLabelTypeBM:
                return AnyLabel::create(CCLabelBMFont::create(txt, this->m_pMenu->m_sFont));
                break;
            
            case kAnyLabelTypeTTF:
                return AnyLabel::create(CCLabelTTF::create(
                    txt, this->m_pMenu->m_sFont, this->m_pMenu->m_fTTFFontSize
                ));
                break;
        }
    }
    return AnyLabel::create(CCLabelTTF::create(txt, "Segoe UI", 16));
}

void ContextMenuItem::activate() {}
void ContextMenuItem::drag(float) {}


bool KeybindContextMenuItem::init(ContextMenu* menu, keybind_id const& id) {
    if (!ContextMenuItem::init(menu))
        return false;
    
    this->m_sID = id;

    this->m_pLabel = this->createLabel();
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
    this->m_pLabel->limitLabelWidth(
        this->getScaledContentSize().width - 10.0f,
        this->m_pMenu ? this->m_pMenu->m_fFontScale : .4f,
        .02f
    );

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

KeybindContextMenuItem* KeybindContextMenuItem::create(
    ContextMenu* menu, keybind_id const& id
) {
    auto ret = new KeybindContextMenuItem;

    if (ret && ret->init(menu, id)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}


bool SpecialContextMenuItem::init(
    ContextMenu* menu,
    const char* spr,
    const char* txt,
    SpecialContextMenuItem::Callback cb
) {
    if (!ContextMenuItem::init(menu))
        return false;
    
    if (spr) {
        this->m_pSprite = CCSprite::createWithSpriteFrameName(spr);
        this->addChild(this->m_pSprite);
    }

    this->m_pLabel = this->createLabel(txt);
    this->m_pLabel->setColor(to3B(g_colText));
    this->addChild(this->m_pLabel);

    this->m_pCallback = cb;

    return true;
}

void SpecialContextMenuItem::visit() {
    auto hasLabel = this->m_pLabel->getString() && strlen(this->m_pLabel->getString());
    if (hasLabel) {
        this->m_pLabel->limitLabelWidth(
            this->getContentSize().width - 30.f,
            this->m_pMenu ? this->m_pMenu->m_fFontScale : .4f,
            .02f
        );
        this->m_pLabel->limitLabelHeight(
            this->getContentSize().height - 3.f,
            this->m_pMenu ? this->m_pMenu->m_fFontScale : .4f,
            .02f
        );
    }

    if (this->m_pSprite) {
        limitSpriteSize(this->m_pSprite,
            this->getContentSize() - 4.f, this->m_fSpriteScale, .02f
        );
    }

    if (hasLabel) {
        this->m_pLabel->setPosition({
            this->getContentSize().width / 2 + (this->m_pSprite ? (
                1.0f + this->m_pSprite->getScaledContentSize().width / 2
            ) : 0.f),
            this->getContentSize().height / 2
        });
    }

    if (this->m_pSprite) {
        this->m_pSprite->setPosition({
            this->getContentSize().width / 2 + (hasLabel ? (
                - 1.0f - this->m_pLabel->getScaledContentSize().width / 2 
            ) : 0.f),
            this->getContentSize().height / 2
        });
    }

    ContextMenuItem::visit();
}

void SpecialContextMenuItem::activate() {
    if (this->m_pCallback) {
        if (!this->m_pCallback(this)) {
            SuperMouseManager::get()->releaseCapture(this);
            if (this->m_pMenu)
                this->m_pMenu->hide();
        }
    }
}

void SpecialContextMenuItem::setSpriteOpacity(GLubyte b) {
    this->m_nSpriteOpacity = b;
    if (this->m_pSprite)
        this->m_pSprite->setOpacity(b);
}

SpecialContextMenuItem* SpecialContextMenuItem::create(
    ContextMenu* menu,
    const char* spr,
    const char* txt,
    SpecialContextMenuItem::Callback cb
) {
    auto ret = new SpecialContextMenuItem;

    if (ret && ret->init(menu, spr, txt, cb)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool DragContextMenuItem::init(
    ContextMenu* menu,
    const char* spr,
    const char* txt,
    DragContextMenuItem::DragCallback cb,
    DragContextMenuItem::DragValue cv
) {
    if (!SpecialContextMenuItem::init(menu, spr, txt, nullptr))
        return false;

    this->m_pDragCallback = cb;
    this->m_pDragValue = cv;
    this->m_sText = txt;

    return true;
}

void DragContextMenuItem::visit() {
    if (this->m_pDragValue) {
        this->m_pLabel->setString(
            (
                this->m_sText +
                ": "_s +
                BetterEdit::formatToString(this->m_pDragValue(), 2u)
            ).c_str() 
        );
    }

    SpecialContextMenuItem::visit();
}

void DragContextMenuItem::drag(float val) {
    if (this->m_pDragCallback)
        this->m_pDragCallback(this, val);
}

DragContextMenuItem* DragContextMenuItem::create(
    ContextMenu* menu,
    const char* spr,
    const char* txt,
    DragContextMenuItem::DragCallback cb,
    DragContextMenuItem::DragValue cv
) {
    auto ret = new DragContextMenuItem;

    if (ret && ret->init(menu, spr, txt, cb, cv)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}


bool PropContextMenuItem::init(
    ContextMenu* menu, PropContextMenuItem::Type type
) {
    if (!ContextMenuItem::init(menu))
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

PropContextMenuItem* PropContextMenuItem::create(
    ContextMenu* menu, PropContextMenuItem::Type type
) {
    auto ret = new PropContextMenuItem;

    if (ret && ret->init(menu, type)) {
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
    this->setScale(1.5f);
    this->m_pEditor = LevelEditorLayer::get();

    this->m_mConfig = {
        { kStateNoneSelected, {
            {{ 
                "betteredit.select_all_left",
                "betteredit.select_all",
                "betteredit.select_all_right",
            }},
            {{ "gd.edit.paste", "betteredit.toggle_ui" }},
        } },

        { kStateOneSelected, {
            {{ "betteredit.preview_mode" }},
            {{ "gd.edit.deselect" }},
        } },

        { kStateManySelected, {
            {{
                "betteredit.edit_object",
                "betteredit.edit_group",
                "betteredit.edit_special"
            }},
            {{ "betteredit.align_x", "betteredit.align_y" }},
            {{ "gd.edit.deselect" }},
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
    this->m_obLocation = pos;
    this->updatePosition();
    this->setVisible(true);
}

void ContextMenu::updatePosition() {
    auto pos = this->m_obLocation;

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto csize = this->getContentSize();
    auto cssize = this->getScaledContentSize();
    auto x = pos.x + cssize.width / 2 - csize.width / 2;
    auto y = pos.y - cssize.height + cssize.height / 2 - csize.height / 2;
    while (x + cssize.width > winSize.width)
        x -= 20.f;
    while (y - cssize.height < 0)
        y += 20.f;

    this->setPosition(x, y);
    this->setSuperMouseHitOffset(csize / 2);
    this->setSuperMouseHitSize(cssize);
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

void ContextMenu::keyDownSuper(enumKeyCodes code) {
    if (code == KEY_Escape) this->hide();
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

    auto h = 0.f;
    auto w = 120.0f;

    float y = 0.0f;
    for (auto const& line : c) {
        float x = 0.0f;
        for (auto const& item : line.items) {
            ContextMenuItem* pItem = nullptr;
            switch (item.m_eType) {
                case ContextMenuStorageItem::kItemTypeKeybind:
                    pItem = KeybindContextMenuItem::create(this, item.m_sKeybindID);
                    break;
                case ContextMenuStorageItem::kItemTypeProperty:
                    pItem = PropContextMenuItem::create(this, item.m_ePropType);
                    break;
            }
            if (pItem) {
                pItem->setPosition(x, y);
                pItem->setContentSize({ w / line.items.size(), line.height });
                this->addChild(pItem);
            }
            x += w / line.items.size();
        }
        y += line.height;
        h += line.height;
    }

    CCARRAY_FOREACH_B_TYPE(this->m_pChildren, node, CCNode) {
        node->setPositionY(
            h - 4.f - node->getPositionY()
        );
    }

    static const float arrowSize = 20.f;
    static const float optionSize = w / 2 - 20.f;

    auto arrowLeft = SpecialContextMenuItem::create(
        this, "BE_ri_arrow_001.png", "", [](auto btn) -> bool { return false; }
    );
    arrowLeft->setPosition(0, 0);
    arrowLeft->setContentSize({ arrowSize, 8.f });
    arrowLeft->m_pSprite->setFlipX(true);
    arrowLeft->m_fSpriteScale = .3f;
    this->addChild(arrowLeft);

    auto arrowRight = SpecialContextMenuItem::create(
        this, "BE_ri_arrow_001.png", "", [](auto btn) -> bool { return true; }
    );
    arrowRight->setPosition(w - arrowSize, 0);
    arrowRight->setContentSize({ arrowSize, 8.f });
    arrowRight->m_fSpriteScale = .3f;
    this->addChild(arrowRight);

    auto settings = SpecialContextMenuItem::create(
        this, "BE_ri_gear_001.png", "Customize"
    );
    settings->setPosition(arrowSize, 0);
    settings->setContentSize({ optionSize, 8.f });
    settings->setSpriteOpacity(95);
    this->addChild(settings);

    auto scaleLabel = DragContextMenuItem::create(
        this, nullptr, "Scale", [this](auto, float v) -> void {
            this->setScale(clamp(this->getScale() + v / 5, .5f, 4.f));
            this->updatePosition();
        }, [this]() -> float {
            return this->getScale();
        }
    );
    scaleLabel->setPosition(optionSize + arrowSize, 0);
    scaleLabel->setContentSize({ optionSize, 8.f });
    this->addChild(scaleLabel);

    h += 8.f;

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
