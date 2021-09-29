#include "ContextMenu.hpp"
#include "CustomizeCMLayer.hpp"
#include "../EditorLayerInput/editorLayerInput.hpp"

#define MORD(x, d) (this->m_pMenu ? this->m_pMenu->x : d)

constexpr const int CMENU_TAG = 600;

bool ContextMenu::init() {
    if (!CCLayerColor::initWithColor(m_colBG, 100.0f, 60.0f))
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
                { PropContextMenuItem::kTypePositionY },
                { PropContextMenuItem::kTypeRotate },
                { PropContextMenuItem::kTypeScale }
            }, 24.f},
            // {{ { ContextMenuStorageItem::kItemTypeQuickGroup } }},
            {{
                { PropContextMenuItem::kTypeZOrder },
                { PropContextMenuItem::kTypeELayer },
                { PropContextMenuItem::kTypeELayer2 },
                { PropContextMenuItem::kTypeColor1 },
                { PropContextMenuItem::kTypeColor2 },
            }, 14.f},
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
                { PropContextMenuItem::kTypePositionY },
                { PropContextMenuItem::kTypeRotate },
                { PropContextMenuItem::kTypeScale }
            }, 24.f},
            // {{ { ContextMenuStorageItem::kItemTypeQuickGroup } }},
            {{
                { PropContextMenuItem::kTypeZOrder },
                { PropContextMenuItem::kTypeELayer },
                { PropContextMenuItem::kTypeELayer2 },
                { PropContextMenuItem::kTypeColor1 },
                { PropContextMenuItem::kTypeColor2 },
            }, 14.f},
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
        ccDrawColor4B(m_colBorder);
        ccDrawRect(
            { 0, 0 },
            this->getContentSize()
        );
    }
}

void ContextMenu::show() {
    this->show(getMousePos());
}

void ContextMenu::show(CCPoint const& pos) {
    if (!BetterEdit::getEnableExperimentalFeatures())
        return;

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
    while (x + cssize.width - cssize.width / 2 + csize.width / 2 > winSize.width)
        x -= 2.f;
    while (y - cssize.height / 2 + csize.height / 2 < 0)
        y += 2.f;

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
    return true;
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
                case ContextMenuStorageItem::kItemTypeQuickGroup:
                    pItem = QuickGroupContextMenuItem::create(this);
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
        this, nullptr, "Scale", 25.f,
        .5f, 2.5f,
        [this](auto, float v) -> void {
            this->setScale(v);
            this->updatePosition();
        },
        [this]() -> float {
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
            if (!obj->m_bIsSelected) {
                this->m_bDeselectObjUnderMouse = true;
                this->m_pEditor->m_pEditorUI->selectObjects(CCArray::createWithObject(obj), false);
            } else {
                this->m_bDeselectObjUnderMouse = false;
            }
            obj->selectObject(m_colSelect);
            if (this->m_pObjSelectedUnderMouse) {
                this->m_pObjSelectedUnderMouse->release();
            }
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
