#include "ContextMenu.hpp"
#include "CustomizeCMLayer.hpp"
#include "../EditorLayerInput/editorLayerInput.hpp"

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

ccColor4B invert4B(ccColor4B const& c) {
    return {
        static_cast<GLubyte>(255 - c.r),
        static_cast<GLubyte>(255 - c.g),
        static_cast<GLubyte>(255 - c.b),
        c.a
    };
}

void limitSpriteSize(CCSprite* spr, CCSize const& size, float def, float min) {
    spr->setScale(1.f);
    auto [cwidth, cheight] = spr->getContentSize();

    float scale = def;
    if (size.height && size.height < cheight) {
        scale = size.height / cheight;
    }
    if (size.width && size.width < cwidth) {
        if (size.width / cwidth < scale)
            scale = size.width / cwidth;
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

bool operator==(CCPoint const&, CCPoint const&);

GameObject* firstObject(CCArray* objs) {
    if (objs->count())
        return as<GameObject*>(objs->objectAtIndex(0));
    return nullptr;
}

float dampenf(float x, float y) {
    return x + y / 10.f;
}

constexpr size_t operator"" _h (const char* txt, size_t) {
    return hash(txt);
}

CCRect getObjectRect(CCNode* node) {
    auto pos = node->getPosition();
    auto size = node->getScaledContentSize();

    return {
        pos.x - size.width / 2,
        pos.y - size.height / 2,
        size.width,
        size.height
    };
}

bool objectsAreAdjacent(GameObject* o1, GameObject* o2) {
    auto rect1 = getObjectRect(o1);
    auto rect2 = getObjectRect(o2);

    return (
        rect1.origin.x <= rect2.origin.x + rect2.size.width &&
        rect2.origin.x <= rect1.origin.x + rect1.size.width &&
        rect1.origin.y <= rect2.origin.y + rect2.size.height &&
        rect2.origin.y <= rect1.origin.y + rect1.size.height
    );
}

void recursiveAddNear(EditorUI* ui, GameObject* fromObj, std::vector<GameObject*> const& vnear, CCArray* arr) {
    for (auto const& obj : vnear) {
        if (objectsAreAdjacent(fromObj, obj)) {
            if (!arr->containsObject(obj)) {
                arr->addObject(obj);
                recursiveAddNear(ui, obj, vnear, arr);
            }
        }
    }
}

void selectStructure(EditorUI* ui, GameObject* fromObj) {
    std::vector<GameObject*> nearby;
    auto pos = fromObj->getPosition();
    CCARRAY_FOREACH_B_TYPE(ui->m_pEditorLayer->getAllObjects(), obj, GameObject) {
        switch (obj->m_nObjectType) {
            case kGameObjectTypeSlope:
            case kGameObjectTypeSolid:
            case kGameObjectTypeSpecial:
            case kGameObjectTypeDecoration:
            case kGameObjectTypeHazard:
            case kGameObjectTypeGravityPad:
            case kGameObjectTypePinkJumpPad:
            case kGameObjectTypeYellowJumpPad:
            case kGameObjectTypeRedJumpPad:
                if (
                    obj->m_nEditorLayer == fromObj->m_nEditorLayer
                ) {
                    if (ccpDistance(obj->getPosition(), pos) < 500.0f) {
                        nearby.push_back(obj);
                    }
                }
                break;
        }
    }
    auto arr = CCArray::create();
    arr->addObject(fromObj);
    recursiveAddNear(ui, fromObj, nearby, arr);
    ui->deselectAll();
    ui->selectObjects(arr, false);
}

constexpr const int CMENU_TAG = 600;

#define MORD(x, d) (this->m_pMenu ? this->m_pMenu->x : d)
#define PROP_GET(...) \
    this->m_getValue = [this](CCArray* objs) -> float { __VA_ARGS__; };
#define PROP_SET(...) \
    this->m_setValue = [this](CCArray* objs, float f, bool abs) -> void { __VA_ARGS__; };
#define PROP_NAME(name) \
    this->m_getName = [this]() -> std::string { return name; };
#define PROP_DRAW(...) \
    this->m_drawCube = [this]() -> void { __VA_ARGS__; };
#define PROP_USABLE(...) \
    this->m_usable = [this]() -> bool { __VA_ARGS__; };
#define PROP_UNDO(...) \
    this->m_undo = [this]() -> void { __VA_ARGS__; };

ccColor4B g_colBG = { 0, 0, 0, 200 };
ccColor4B g_colText = { 255, 255, 255, 255 };
ccColor4B g_colGray = { 150, 150, 150, 255 };
ccColor4B g_colHover = { 255, 255, 255, 52 };
ccColor4B g_colTransparent = { 0, 0, 0, 0 };
ccColor3B g_colSelect = { 255, 194, 90 };

bool ContextMenuItem::init(ContextMenu* menu) {
    if (!CCNode::init())
        return false;
    
    this->m_pMenu = menu;
    
    return true;
}

void ContextMenuItem::draw() {
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

    // some mf somewhere resets the blend
    // func sometimes
    ccGLBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ccDrawSolidRect(
        { 0, 0 },
        this->getScaledContentSize(),
        to4f(col)
    );

    auto mul = this->m_pMenu ? this->m_pMenu->getScale() : 1.f;

    this->setSuperMouseHitSize(this->getScaledContentSize() * mul);
    this->setSuperMouseHitOffset(this->getScaledContentSize() / 2 * mul);

    CCNode::draw();
}

bool ContextMenuItem::mouseDownSuper(MouseButton btn, CCPoint const& mpos) {
    if (!this->m_bDisabled && btn == kMouseButtonLeft) {
        if (this->m_pMenu) this->m_pMenu->deactivateOthers(this);
        this->activate();
        this->m_obLastMousePos = mpos;
        SuperMouseManager::get()->captureMouse(this);
    }
    if (this->m_bDisabled) {
        this->m_obLastMousePos = mpos;
    }
    return true;
}

bool ContextMenuItem::mouseUpSuper(MouseButton btn, CCPoint const&) {
    if (!this->m_bDisabled && btn == kMouseButtonLeft) {
        SuperMouseManager::get()->releaseCapture(this);
    }
    return true;
}

void ContextMenuItem::mouseMoveSuper(CCPoint const& mpos) {
    if (!this->m_bDisabled && this->m_bSuperMouseDown) {
        this->drag(mpos.y - this->m_obLastMousePos.y);
        this->m_obLastMousePos = mpos;
    }
    if (this->m_bDisabled && this->m_bSuperMouseDown) {
        this->setPosition(
            this->getPosition() + (this->m_obLastMousePos - mpos) * MORD(getScale(), 1.f)
        );
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
void ContextMenuItem::deactivate() {}
void ContextMenuItem::updateItem() {}
void ContextMenuItem::drag(float) {}
void ContextMenuItem::hide() { this->deactivate(); }


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


ActionContextMenuItem::ACMIAction ActionContextMenuItem::actionForID(std::string const& id) {
    switch (hash(id.c_str())) {
        case "deselect_this"_h: return {
            "Deselect This",
            [](ActionContextMenuItem* item, GameObject* obj) -> void {
                auto ui = LevelEditorLayer::get()->getEditorUI();
                CATCH_NULL(ui->m_pSelectedObjects)->removeObject(obj);
                if (ui->m_pSelectedObject) {
                    ui->m_pSelectedObject = nullptr;
                }
                if (item->m_pMenu) {
                    item->m_pMenu->m_pObjSelectedUnderMouse = nullptr;
                }
                obj->deselectObject();
                ui->updateButtons();
                ui->updateObjectInfoLabel();
            }
        };
        case "select_structure"_h: return {
            "Select Structure",
            [](ActionContextMenuItem* item, GameObject* obj) -> void {
                auto ui = LevelEditorLayer::get()->getEditorUI();
                selectStructure(ui, obj);
                ui->updateButtons();
                ui->updateObjectInfoLabel();
                if (item->m_pMenu) {
                    item->m_pMenu->m_bDeselectObjUnderMouse = false;
                }
            }
        };
        default:
            return { "Unknown Action" };
    }
}

bool ActionContextMenuItem::init(ContextMenu* menu, std::string const& id) {
    if (!ContextMenuItem::init(menu))
        return false;
    
    this->m_obAction = this->actionForID(id);

    this->m_pLabel = this->createLabel(this->m_obAction.m_sName.c_str());
    this->m_pLabel->setColor(to3B(g_colText));
    this->addChild(this->m_pLabel);

    return true;
}

void ActionContextMenuItem::visit() {
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

void ActionContextMenuItem::activate() {
    if (this->m_pMenu) {
        this->m_obAction.m_callback(this, this->m_pMenu->m_pObjSelectedUnderMouse);
        this->m_pMenu->hide();
    } else {
        this->m_obAction.m_callback(this, nullptr);
    }
    SuperMouseManager::get()->releaseCapture(this);
}

ActionContextMenuItem* ActionContextMenuItem::create(
    ContextMenu* menu, std::string const& id
) {
    auto ret = new ActionContextMenuItem;

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
        if (this->m_pSprite)
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
    
    this->m_pValueLabel = this->createLabel();
    this->addChild(this->m_pValueLabel);
    this->m_eType = type;

    this->m_pInput = CCTextInputNode::create("", this, "bigFont.fnt", 100.0f, 10.0f);
    this->m_pInput->setDelegate(this);
    this->m_pInput->setAllowedChars(inputf_Numeral);
    this->m_pInput->retain();

    this->m_pCube = CCSprite::createWithSpriteFrameName("d_largeSquare_02_001.png");
    this->m_pCube->setVisible(false);
    this->m_pCube->setScale(.18f);
    this->addChild(this->m_pCube);

    this->m_pCube2 = CCSprite::createWithSpriteFrameName("d_largeSquare_02_001.png");
    this->m_pCube2->setVisible(false);
    this->m_pCube2->setScale(.18f);
    this->addChild(this->m_pCube2);

    this->superKeyPopSelf();

    switch (this->m_eType) {
        case kTypeScale: {
            PROP_GET(
                auto max_val = 0.0f;
                CCARRAY_FOREACH_B_BASE(objs, obj, GameObject*, ix) {
                    if (ix == 0) {
                        max_val = obj->getScale();
                    } else if (obj->getScale() > max_val) {
                        max_val = obj->getScale();
                    }
                }
                return max_val;
            );
            PROP_SET(
                auto ui = LevelEditorLayer::get()->getEditorUI();
                ui->m_pScaleControl->loadValues(nullptr, objs);
                if (this->absoluteModifier()) {
                    if (abs) {
                        CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject) {
                            auto sobjs = CCArray::createWithObject(obj);
                            ui->scaleObjects(
                                sobjs, f,
                                obj->getPosition()
                            );
                        }
                    } else {
                        CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject) {
                            auto sobjs = CCArray::createWithObject(obj);
                            ui->scaleObjects(
                                sobjs, dampenf(this->m_getValue(objs), f), 
                                obj->getPosition()
                            );
                        }
                    }
                } else {
                    if (abs) {
                        ui->scaleObjects(
                            objs, f,
                            this->m_obScaleCenter
                        );
                    } else {
                        ui->scaleObjects(
                            objs, dampenf(this->m_getValue(objs), f),
                            this->m_obScaleCenter
                        );
                    }
                }
            );
            PROP_USABLE(
                auto objs = LevelEditorLayer::get()->getEditorUI()->getSelectedObjects();
                if (!objs->count()) 
                    return false;
                return true;
            );
            PROP_UNDO(
                auto ui = EditorUI::get();
                auto undo = UndoObject::createWithTransformObjects(
                    ui->getSelectedObjects(), kUndoCommandTransform
                );
                ui->m_pEditorLayer->m_pUndoObjects->addObject(undo);
            );
            PROP_NAME("Scale");
            PROP_DRAW(
                auto scale = this->getValue() / this->m_fLastDraggedValue * .18f;
                scale = clamp(scale, .1f, .25f);
                this->m_pCube->setScale(scale);
            );
            this->m_pCube2->setOpacity(50);
            this->m_pCube->setVisible(true);
            this->m_pCube2->setVisible(true);
            this->m_pInput->setAllowedChars(inputf_NumeralFloatSigned);
            this->m_sSuffix = "x";
            this->m_fDefaultValue = 1.f;
        } break;
            
        case kTypeRotate: {
            PROP_GET(return firstObject(objs)->getRotation());
            PROP_SET(
                auto ui = LevelEditorLayer::get()->getEditorUI();
                if (this->absoluteModifier()) {
                    if (abs) {
                        CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject) {
                            auto sobjs = CCArray::createWithObject(obj);
                            ui->rotateObjects(
                                sobjs, f - obj->getRotation(),
                                ui->getGroupCenter(sobjs, false)
                            );
                        }
                    } else {
                        CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject) {
                            auto sobjs = CCArray::createWithObject(obj);
                            ui->rotateObjects(
                                sobjs, f * 2.f,
                                ui->getGroupCenter(sobjs, false)
                            );
                        }
                    }
                } else {
                    if (abs) {
                        ui->rotateObjects(
                            objs, f - this->m_getValue(objs),
                            ui->getGroupCenter(objs, false)
                        );
                    } else {
                        ui->rotateObjects(
                            objs, f * 2.f,
                            ui->getGroupCenter(objs, false)
                        );
                    }
                }
            );
            PROP_USABLE(
                auto objs = LevelEditorLayer::get()->getEditorUI()->getSelectedObjects();
                if (!objs->count()) 
                    return false;
                CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject) {
                    if (obj->m_nObjectType == kGameObjectTypeSolid ||
                        obj->m_nObjectType == kGameObjectTypeSlope)
                        return false;
                }
                return true;
            );
            PROP_UNDO(
                auto ui = EditorUI::get();
                auto undo = UndoObject::createWithTransformObjects(
                    ui->getSelectedObjects(), kUndoCommandTransform
                );
                ui->m_pEditorLayer->m_pUndoObjects->addObject(undo);
            );
            PROP_NAME("Rotate");
            PROP_DRAW(
                this->m_pCube2->setRotation(this->m_fLastDraggedValue);
                this->m_pCube->setRotation(this->getValue());
            );
            this->m_sSuffix = "°";
            this->m_pCube2->setOpacity(50);
            this->m_pCube->setVisible(true);
            this->m_pCube2->setVisible(true);
            this->m_pInput->setAllowedChars(inputf_NumeralFloatSigned);
        } break;
            
        case kTypePositionX: {
            PROP_GET(return firstObject(objs)->getPositionX());
            PROP_SET(
                auto ui = LevelEditorLayer::get()->getEditorUI();
                CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject) {
                    if (abs)
                        obj->setPositionX(f);
                    else
                        obj->setPositionX(obj->getPositionX() + f);
                }
            );
            PROP_USABLE(
                auto objs = LevelEditorLayer::get()->getEditorUI()->getSelectedObjects();
                return objs->count();
            );
            PROP_UNDO(
                auto ui = EditorUI::get();
                auto undo = UndoObject::createWithTransformObjects(
                    ui->getSelectedObjects(), kUndoCommandTransform
                );
                ui->m_pEditorLayer->m_pUndoObjects->addObject(undo);
            );
            PROP_NAME("Position X");
            PROP_DRAW(
                auto pos = this->getValue() / this->m_fLastDraggedValue;
                pos = clamp(pos, -10.f, 10.f);
                this->m_pCube->setPositionX(
                    this->m_pCube->getPositionX() + pos
                );
            );
            this->m_pCube2->setOpacity(50);
            this->m_pCube->setVisible(true);
            this->m_pCube2->setVisible(true);
            this->m_pInput->setAllowedChars(inputf_NumeralFloatSigned);
        } break;

        case kTypePositionY: {
            PROP_GET(return firstObject(objs)->getPositionY());
            PROP_SET(
                auto ui = LevelEditorLayer::get()->getEditorUI();
                CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject) {
                    if (abs)
                        obj->setPositionY(f);
                    else
                        obj->setPositionY(obj->getPositionY() + f);
                }
            );
            PROP_USABLE(
                auto objs = LevelEditorLayer::get()->getEditorUI()->getSelectedObjects();
                return objs->count();
            );
            PROP_UNDO(
                auto ui = EditorUI::get();
                auto undo = UndoObject::createWithTransformObjects(
                    ui->getSelectedObjects(), kUndoCommandTransform
                );
                ui->m_pEditorLayer->m_pUndoObjects->addObject(undo);
            );
            PROP_NAME("Position Y");
            PROP_DRAW(
                auto pos = this->getValue() / this->m_fLastDraggedValue;
                pos = clamp(pos, -10.f, 10.f);
                this->m_pCube->setPositionY(
                    this->m_pCube->getPositionY() + pos
                );
            );
            this->m_pCube2->setOpacity(50);
            this->m_pCube->setVisible(true);
            this->m_pCube2->setVisible(true);
            this->m_pInput->setAllowedChars(inputf_NumeralFloatSigned);
        } break;
    }

    this->m_fLastDraggedValue = this->getValue();

    return true;
}

bool PropContextMenuItem::mouseDownSuper(MouseButton btn, CCPoint const& pos) {
    if (!this->isUsable())
        return true;
    if (this->m_bEditingText && this->m_bTextSelected) {
        this->enableInput(false);
        return true;
    } else {
        if (btn == kMouseButtonLeft && pos == this->m_obLastMousePos) {
            if (!this->m_bEditingText) {
                this->enableInput(true);
            } else {
                this->m_bTextSelected = true;
            }
            return true;
        } else {
            this->enableInput(false);
        }
    }
    if (this->m_undo) {
        this->m_undo();
    }
    return this->ContextMenuItem::mouseDownSuper(btn, pos);
}

bool PropContextMenuItem::mouseUpSuper(MouseButton btn, CCPoint const& pos) {
    if (!this->isUsable())
        return true;
    this->m_fLastDraggedValue = this->getValue();
    return this->ContextMenuItem::mouseUpSuper(btn, pos);
}

void PropContextMenuItem::mouseDownOutsideSuper(MouseButton, CCPoint const&) {
    this->enableInput(false);
}

float PropContextMenuItem::getValue() {
    if (m_getValue) {
        auto objs = LevelEditorLayer::get()->getEditorUI()->getSelectedObjects();
        if (objs->count())
            return m_getValue(objs);
    }
    return m_fDefaultValue;
}

void PropContextMenuItem::setValue(float f, bool abs) {
    if (m_setValue) {
        auto objs = LevelEditorLayer::get()->getEditorUI()->getSelectedObjects();
        CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject)
            return m_setValue(objs, f, abs);
    }
}

bool PropContextMenuItem::isUsable() {
    if (m_usable) return m_usable();
    return true;
}

bool PropContextMenuItem::absoluteModifier() {
    return CCDirector::sharedDirector()
        ->getKeyboardDispatcher()
        ->getAltKeyPressed();
}

bool PropContextMenuItem::smallModifier() {
    return CCDirector::sharedDirector()
        ->getKeyboardDispatcher()
        ->getShiftKeyPressed();
}

void PropContextMenuItem::drag(float val) {
    if (!this->isUsable())
        return;
    if (this->smallModifier())
        val /= 5.f;
    auto objs = LevelEditorLayer::get()->getEditorUI()->getSelectedObjects();
    CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject) {
        this->m_setValue(objs, val, false);
    }
}

void PropContextMenuItem::enableInput(bool b) {
    this->m_bEditingText = b;
    if (b) {
        this->m_pInput->setString(
            BetterEdit::formatToString(this->getValue(), 2u).c_str()
        );
        this->m_pInput->m_pTextField->attachWithIME();
        this->superKeyPushSelf();
    } else {
        this->m_pInput->m_pTextField->detachWithIME();
        this->m_bTextSelected = false;
        this->superKeyPopSelf();
    }
}

void PropContextMenuItem::textChanged(CCTextInputNode*) {
    auto str = this->m_pInput->getString();
    if (str && strlen(str)) {
        if (this->m_bTextSelected) {
            auto c = str[strlen(str) - 1];
            this->setValue(this->m_fDefaultValue, true);
            this->m_bTextSelected = false;
            this->m_pInput->setString(std::string(1, c).c_str());
            return;
        }
        this->m_bTextSelected = false;
        try {
            bool digit = false;
            for (size_t i = 0; i < strlen(str); i++) {
                if (isdigit(str[i])) digit = true;
            }
            if (digit) {
                this->setValue(std::stof(str), true);
            } else {
                this->setValue(this->m_fDefaultValue, true);
            }
        } catch (...) {
            this->setValue(this->m_fDefaultValue, true);
        }
    }
}

bool PropContextMenuItem::keyDownSuper(enumKeyCodes key) {
    this->m_bTextSelected = false;
    if (key == KEY_Escape || key == KEY_Enter)
        this->enableInput(false);
    return true;
}

void PropContextMenuItem::visit() {
    if (this->m_bEditingText) {
        this->m_pValueLabel->setString(
            std::string(this->m_pInput->getString() + this->m_sSuffix).c_str()
        );
    } else {
        auto text = this->m_getName() + ": " +
            BetterEdit::formatToString(this->getValue(), 2u) +
            this->m_sSuffix;
        this->m_pValueLabel->setString(text.c_str());
    }

    bool image = this->m_drawCube && this->getScaledContentSize().height >= 15.f;

    this->m_pValueLabel->limitLabelWidth(
        this->getContentSize().width - 30.f,
        MORD(m_fFontScale, .4f) - (image ? .05f : 0.f),
        .02f
    );
    this->m_pValueLabel->limitLabelHeight(
        this->getContentSize().height - 3.f,
        MORD(m_fFontScale, .4f) - (image ? .05f : 0.f),
        .02f
    );

    if (image) {
        this->m_pValueLabel->setPosition(
            this->getContentSize().width / 2,
            6.f
        );
    } else {
        this->m_pValueLabel->setPosition(
            this->getContentSize() / 2
        );
    }

    ContextMenuItem::visit();
}

void PropContextMenuItem::draw() {
    if (!this->isUsable()) {
        this->m_pValueLabel->setOpacity(50);
        this->m_pCube->setOpacity(50);
    } else {
        this->m_pValueLabel->setOpacity(255);
        this->m_pCube->setOpacity(255);
    }

    if (this->m_drawCube && this->getScaledContentSize().height >= 15.f) {
        auto pos = this->getScaledContentSize() / 2;
        pos.height += 4.f;
        this->m_pCube->setPosition(pos);
        this->m_pCube2->setPosition(pos);
        this->m_drawCube();
    }

    if (this->m_bEditingText) {
        if (this->m_bTextSelected) {
            auto pos = this->m_pValueLabel->getPosition();
            auto size = this->m_pValueLabel->getScaledContentSize();
            ccDrawSolidRect(
                pos - size / 2, pos + size / 2, to4f(g_colText)
            );
            this->m_pValueLabel->setColor(to3B(invert4B(g_colText)));
        } else {
            auto pos = this->m_pValueLabel->getPosition();
            auto size = this->m_pValueLabel->getScaledContentSize();
            ccDrawSolidRect(
                pos + size / 2,
                pos + CCPoint { size.width / 2 + 0.5f, -size.height / 2 },
                to4f(g_colText)
            );
            this->m_pValueLabel->setColor(to3B(g_colText));
        }
    } else {
        this->m_pValueLabel->setColor(to3B(g_colText));
    }

    ContextMenuItem::draw();
}

void PropContextMenuItem::deactivate() {
    this->enableInput(false);
}

void PropContextMenuItem::updateItem() {
    if (this->m_pMenu) {
        auto ui = this->m_pMenu->m_pEditor->m_pEditorUI;

        if (!ui) return;

        auto objs = ui->getSelectedObjects();

        this->m_obScaleCenter =
            ui->getGroupCenter(objs, false);

        this->m_pMenu->m_pEditor->m_pEditorUI->m_obScalePos = this->m_obScaleCenter;
    }
}

PropContextMenuItem::~PropContextMenuItem() {
    this->m_pInput->release();
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

    this->m_vDefaultConfig = {
        {{ 
            "betteredit.select_all_left",
            "betteredit.select_all",
            "betteredit.select_all_right",
        }},
        {{
            { "betteredit.preview_mode" },
            { "betteredit.rotate_saws" },
        }},
        {{ "gd.edit.paste", "betteredit.toggle_ui" }},
        {{ "betteredit.screenshot" }},
    };

    this->m_mContexts = {
        { kContextTypeObject, {{
            {{
                { PropContextMenuItem::kTypePositionX },
                { PropContextMenuItem::kTypePositionY }
            }, 24.f},
            {{
                { PropContextMenuItem::kTypeRotate },
                { PropContextMenuItem::kTypeScale }
            }, 24.f},
            {{
                "betteredit.edit_object",
                "betteredit.edit_group",
                "betteredit.edit_special"
            }},
            {{ "gd.edit.deselect", "deselect_this"_s }},
            {{ "select_structure"_s }},
        }, {
            {{
                { PropContextMenuItem::kTypePositionX },
                { PropContextMenuItem::kTypePositionY }
            }, 24.f},
            {{
                { PropContextMenuItem::kTypeRotate },
                { PropContextMenuItem::kTypeScale }
            }, 24.f},
            {{
                "betteredit.edit_object",
                "betteredit.edit_group",
                "betteredit.edit_special"
            }},
            {{ "betteredit.align_x", "betteredit.align_y" }},
            {{ "gd.edit.deselect", "deselect_this"_s }},
            {{ "select_structure"_s }},
        }} },
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
    while (y < 0)
        y += 20.f;

    this->setPosition(x, y);
    this->setSuperMouseHitOffset(csize / 2);
    this->setSuperMouseHitSize(cssize);
}

void ContextMenu::hide() {
    if (this->m_pObjSelectedUnderMouse) {
        if (this->m_bDeselectObjUnderMouse) {
            CATCH_NULL(this->m_pEditor->m_pEditorUI->m_pSelectedObjects)->removeObject(
                this->m_pObjSelectedUnderMouse
            );
            if (this->m_pEditor->m_pEditorUI->m_pSelectedObject) {
                this->m_pEditor->m_pEditorUI->m_pSelectedObject = nullptr;
            }
            this->m_pObjSelectedUnderMouse->deselectObject();
            this->m_pObjSelectedUnderMouse->release();
            this->m_pObjSelectedUnderMouse = nullptr;
            this->m_pEditor->m_pEditorUI->updateButtons();
        } else {
            if (this->m_pObjSelectedUnderMouse->m_bIsSelected)
                this->m_pObjSelectedUnderMouse->selectObject();
        }
    }
    
    this->setVisible(false);
    this->setPosition(-700.0f, -900.0f);
    CCARRAY_FOREACH_B_TYPE(this->m_pChildren, node, ContextMenuItem) {
        node->hide();
    }
}

void ContextMenu::mouseDownOutsideSuper(MouseButton, CCPoint const&) {
    if (!this->m_bDisabled)
        this->hide();
}

bool ContextMenu::mouseDownSuper(MouseButton, CCPoint const&) {
    return false;
}

bool ContextMenu::keyDownSuper(enumKeyCodes code) {
    if (!this->m_bDisabled && code == KEY_Escape) this->hide();
    return false;
}

void ContextMenu::generate(ContextType s, bool multi) {
    this->removeAllChildren();

    auto sel = this->m_pEditor->getEditorUI()->getSelectedObjects();

    Config c;

    if (s != kContextTypeAuto) {
        if (s == kContextTypeDefault)
            c = this->m_vDefaultConfig;
        else
            c = multi ?
                this->m_mContexts[s].m_vMultiConfig :
                this->m_mContexts[s].m_vSingleConfig;
    } else {
        bool multi = sel->count() > 1;

        auto type = this->getTypeUnderMouse();

        if (type == kContextTypeDefault) {
            c = this->m_vDefaultConfig;
        } else {
            if (this->m_mContexts.count(type)) {
                c = multi ?
                    this->m_mContexts[type].m_vMultiConfig :
                    this->m_mContexts[type].m_vSingleConfig;
            } else {
                if (this->m_mContexts.count(kContextTypeObject)) {
                    c = multi ?
                        this->m_mContexts[kContextTypeObject].m_vMultiConfig :
                        this->m_mContexts[kContextTypeObject].m_vSingleConfig;
                } else {
                    c = this->m_vDefaultConfig;
                }
            }
        }
    }

    auto h = 0.f;
    auto w = 120.0f;

    float y = 0.0f;
    for (auto const& line : c) {
        float x = 0.0f;
        int total_grow = 0;
        for (auto const& item : line.items)
            total_grow += item.m_nGrow;
        
        for (auto const& item : line.items) {
            ContextMenuItem* pItem = nullptr;
            switch (item.m_eType) {
                case ContextMenuStorageItem::kItemTypeKeybind:
                    pItem = KeybindContextMenuItem::create(this, item.m_sKeybindID);
                    break;
                case ContextMenuStorageItem::kItemTypeProperty:
                    pItem = PropContextMenuItem::create(this, item.m_ePropType);
                    break;
                case ContextMenuStorageItem::kItemTypeAction:
                    pItem = ActionContextMenuItem::create(this, item.m_sActionID);
                    break;
            }
            if (pItem) {
                pItem->setPosition(x, y);
                pItem->setContentSize({
                    w * (static_cast<float>(item.m_nGrow) / total_grow),
                    line.height
                });
                this->addChild(pItem);
                if (this->m_bDisabled)
                    pItem->m_bDisabled = true;
            }
            x += w * (static_cast<float>(item.m_nGrow) / total_grow);
        }
        y += line.height;
        h += line.height;
    }

    CCARRAY_FOREACH_B_TYPE(this->m_pChildren, node, CCNode) {
        node->setPositionY(
            h + 8.f - node->getPositionY() - node->getScaledContentSize().height
        );
    }

    static const float arrowSize = 20.f;
    static const float optionSize = w / 2 - 20.f;

    auto arrowLeft = SpecialContextMenuItem::create(
        this, "BE_ri_arrow_001.png", "", [](auto btn) -> bool { return false; }
    );
    arrowLeft->setPosition(0, 0);
    arrowLeft->setContentSize({ arrowSize, 8.f });
    CATCH_NULL(arrowLeft->m_pSprite)->setFlipX(true);
    arrowLeft->m_fSpriteScale = .3f;
    arrowLeft->m_bDisabled = this->m_bDisabled;
    this->addChild(arrowLeft);

    auto arrowRight = SpecialContextMenuItem::create(
        this, "BE_ri_arrow_001.png", "", [](auto btn) -> bool { return true; }
    );
    arrowRight->setPosition(w - arrowSize, 0);
    arrowRight->setContentSize({ arrowSize, 8.f });
    arrowRight->m_fSpriteScale = .3f;
    arrowRight->m_bDisabled = this->m_bDisabled;
    this->addChild(arrowRight);

    auto settings = SpecialContextMenuItem::create(
        this, "BE_ri_gear_001.png", "Customize", [](auto) -> bool {
            CustomizeCMLayer::create()->show();
            return false;
        }
    );
    settings->setPosition(arrowSize, 0);
    settings->setContentSize({ optionSize, 8.f });
    settings->setSpriteOpacity(95);
    settings->m_bDisabled = this->m_bDisabled;
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
    scaleLabel->m_bDisabled = this->m_bDisabled;
    this->addChild(scaleLabel);

    h += 8.f;

    this->setContentSize({ w, h });
}

void ContextMenu::deactivateOthers(ContextMenuItem* butNotThis) {
    CCARRAY_FOREACH_B_TYPE(this->m_pChildren, node, ContextMenuItem) {
        if (node != butNotThis)
            node->deactivate();
    }
}

void ContextMenu::updateItems() {
    CCARRAY_FOREACH_B_TYPE(this->m_pChildren, node, ContextMenuItem) {
        node->updateItem();
    }
}

ContextMenu::ContextType ContextMenu::getTypeUnderMouse(bool selectUnder) {
    auto mpos = this->m_pEditor->m_pObjectLayer->convertToNodeSpace(getMousePos());
    auto obj = this->m_pEditor->objectAtPosition(mpos);
    
    if (obj) {
        if (testSelectObjectLayer(obj)) {
            this->m_pEditor->m_pEditorUI->selectObjects(CCArray::createWithObject(obj), false);
            obj->selectObject(g_colSelect);
            if (this->m_pObjSelectedUnderMouse) {
                this->m_pObjSelectedUnderMouse->release();
            }
            this->m_bDeselectObjUnderMouse = true;
            this->m_pObjSelectedUnderMouse = obj;
            this->m_pObjSelectedUnderMouse->retain();
            if (obj->m_nObjectType == kGameObjectTypeSolid ||
                obj->m_nObjectType == kGameObjectTypeSlope)
                return kContextTypeSolid;
            if (obj->m_nObjectType == kGameObjectTypeDecoration)
                return kContextTypeDetail;
            return kContextTypeObject;
        }
    }

    return kContextTypeDefault;
}

ContextMenu* ContextMenu::get(LevelEditorLayer* lel) {
    if (lel) {
        return as<ContextMenu*>(lel->getChildByTag(CMENU_TAG));
    }

    if (!LevelEditorLayer::get())
        return nullptr;
    
    return as<ContextMenu*>(
        LevelEditorLayer::get()
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
