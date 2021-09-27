#include "../ContextMenu.hpp"
#include "../CustomizeCMLayer.hpp"
#include "../../EditorLayerInput/editorLayerInput.hpp"

#define MORD(x, d) (this->m_pMenu ? this->m_pMenu->x : d)
#define PROP_GET(...) \
    this->m_getValue = [this](CCArray* objs, GameObject* specific) -> float { __VA_ARGS__; };
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
#define PROP_VALNAME(...) \
    this->m_specialValueName = [this](float value) -> std::string { __VA_ARGS__; return ""; };

bool PropContextMenuItem::init(
    ContextMenu* menu, PropContextMenuItem::Type type
) {
    if (!ContextMenuItem::init(menu))
        return false;
    
    this->m_pNameLabel = this->createLabel();
    this->addChild(this->m_pNameLabel);

    this->m_pValueLabel = this->createLabel();
    this->addChild(this->m_pValueLabel);
    this->m_eType = type;

    this->m_pAbsModifierLabel = this->createLabel("Alt");
    this->addChild(this->m_pAbsModifierLabel);

    this->m_pSmallModifierLabel = this->createLabel("Shift");
    this->addChild(this->m_pSmallModifierLabel);

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
                                sobjs, dampenf(this->getValue(), f), 
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
                            objs, dampenf(this->getValue(), f),
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
            this->m_fScrollDivisor = 12.f;
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
                            objs, f - this->getValue(),
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
            // so it goes in 5 degrees (24/5)
            this->m_fScrollDivisor = 4.8f;
        } break;
            
        case kTypePositionX: {
            PROP_GET(
                if (specific) return specific->getPositionX();
                return firstObject(objs)->getPositionX()
            );
            PROP_SET(
                auto ui = LevelEditorLayer::get()->getEditorUI();
                CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject) {
                    float pos = m_fDragCollect * 2.f;
                    if (!abs) {
                        pos = this->m_mStartValues[obj] + m_fDragCollect * 2.f;
                    } else {
                        pos = f;
                    }
                    if (this->absoluteModifier()) {
                        auto snap = MORD(m_fObjectMoveGridSnap, 15.f);
                        if (this->smallModifier())
                            snap /= 4.f;
                        pos = roundf(pos / snap) * snap + 15.f;
                    }
                    obj->setPositionX(pos);
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
                auto pos = (this->getValue() - this->m_fLastDraggedValue) / 7.f;
                auto l = this->getScaledContentSize().width / 6.f;
                pos = clamp(pos, -l, l);
                this->m_pCube->setPositionX(
                    this->m_pCube->getPositionX() + pos
                );
            );
            this->m_bHorizontalDrag = true;
            this->m_pCube2->setOpacity(50);
            this->m_pCube->setVisible(true);
            this->m_pCube2->setVisible(true);
            this->m_pInput->setAllowedChars(inputf_NumeralFloatSigned);
        } break;

        case kTypePositionY: {
            PROP_GET(
                if (specific) return specific->getPositionY();
                return firstObject(objs)->getPositionY()
            );
            PROP_SET(
                auto ui = LevelEditorLayer::get()->getEditorUI();
                CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject) {
                    float pos = m_fDragCollect * 2.f;
                    if (!abs) {
                        pos = this->m_mStartValues[obj] + m_fDragCollect * 2.f;
                    } else {
                        pos = f;
                    }
                    if (this->absoluteModifier()) {
                        auto snap = MORD(m_fObjectMoveGridSnap, 15.f);
                        if (this->smallModifier())
                            snap /= 4.f;
                        pos = roundf(pos / snap) * snap + 15.f;
                    }
                    obj->setPositionY(pos);
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
                auto pos = (this->getValue() - this->m_fLastDraggedValue) / 7.f;
                auto l = this->getScaledContentSize().height / 8.f;
                pos = clamp(pos, -l, l);
                this->m_pCube->setPositionY(
                    this->m_pCube->getPositionY() + pos
                );
            );
            this->m_pCube2->setOpacity(50);
            this->m_pCube->setVisible(true);
            this->m_pCube2->setVisible(true);
            this->m_pInput->setAllowedChars(inputf_NumeralFloatSigned);
        } break;
            
        case kTypeELayer: {
            PROP_GET(
                if (specific) return static_cast<float>(specific->m_nEditorLayer);
                return static_cast<float>(firstObject(objs)->m_nEditorLayer)
            );
            PROP_SET(
                CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject) {
                    if (abs) {
                        obj->m_nEditorLayer = static_cast<int>(f);
                    } else {
                        auto val = static_cast<int>(this->m_fDragCollect / 6.f);
                        obj->m_nEditorLayer = static_cast<int>(this->m_mStartValues[obj]) + val;
                    }
                    // todo: make these use max layer from layer manager
                    if (obj->m_nEditorLayer > 500)
                        obj->m_nEditorLayer = 500;
                    if (obj->m_nEditorLayer < 0)
                        obj->m_nEditorLayer = 0;
                }
            );
            PROP_USABLE(
                auto objs = LevelEditorLayer::get()->getEditorUI()->getSelectedObjects();
                if (!objs->count()) 
                    return false;
                return true;
            );
            PROP_NAME("Editor Layer");
            this->m_pInput->setAllowedChars(inputf_Numeral);
            this->m_bHasAbsoluteModifier = false;
            this->m_fScrollDivisor = 2.f;
        } break;
            
        case kTypeELayer2: {
            PROP_GET(
                if (specific) return static_cast<float>(specific->m_nEditorLayer2);
                return static_cast<float>(firstObject(objs)->m_nEditorLayer2)
            );
            PROP_SET(
                CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject) {
                    if (abs) {
                        obj->m_nEditorLayer2 = static_cast<int>(f);
                    } else {
                        auto val = static_cast<int>(this->m_fDragCollect / 6.f);
                        obj->m_nEditorLayer2 = static_cast<int>(this->m_mStartValues[obj]) + val;
                    }
                    // todo: make these use max layer from layer manager
                    if (obj->m_nEditorLayer2 > 500)
                        obj->m_nEditorLayer2 = 500;
                    if (obj->m_nEditorLayer2 < 0)
                        obj->m_nEditorLayer2 = 0;
                }
            );
            PROP_USABLE(
                auto objs = LevelEditorLayer::get()->getEditorUI()->getSelectedObjects();
                if (!objs->count()) 
                    return false;
                return true;
            );
            PROP_NAME("Editor Layer 2");
            this->m_pInput->setAllowedChars(inputf_Numeral);
            this->m_bHasAbsoluteModifier = false;
            this->m_fScrollDivisor = 2.f;
        } break;
            
        case kTypeColor1: {
            PROP_GET(
                if (specific && specific->m_pBaseColor)
                    return static_cast<float>(specific->m_pBaseColor->colorID);
                if (firstObject(objs)->m_pBaseColor)
                    return static_cast<float>(firstObject(objs)->m_pBaseColor->colorID);
                return m_fDefaultValue;
            );
            PROP_SET(
                CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject) {
                    if (!obj->m_pBaseColor) continue;
                    if (abs) {
                        obj->m_pBaseColor->colorID = static_cast<int>(f);
                    } else {
                        auto val = static_cast<int>(this->m_fDragCollect / 6.f);
                        obj->m_pBaseColor->colorID = static_cast<int>(this->m_mStartValues[obj]) + val;
                    }
                    if (obj->m_pBaseColor->colorID > 1023)
                        obj->m_pBaseColor->colorID = 1023;
                    if (obj->m_pBaseColor->colorID < 0)
                        obj->m_pBaseColor->colorID = 0;
                }
            );
            PROP_USABLE(
                auto objs = LevelEditorLayer::get()->getEditorUI()->getSelectedObjects();
                CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject) {
                    if (obj->m_pBaseColor)
                        return true;
                }
                return false;
            );
            PROP_VALNAME(
                if (value == 0) return "Default";
                if (value > 999) return GJSpecialColorSelect::textForColorIdx(static_cast<int>(value));
                
            );
            PROP_NAME("Base Color");
            this->m_pInput->setAllowedChars(inputf_ColorChannel);
            this->m_bHasAbsoluteModifier = false;
            this->m_fScrollDivisor = 2.f;
        } break;
            
        case kTypeColor2: {
            PROP_GET(
                if (specific && specific->m_pDetailColor)
                    return static_cast<float>(specific->m_pDetailColor->colorID);
                if (firstObject(objs)->m_pDetailColor)
                    return static_cast<float>(firstObject(objs)->m_pDetailColor->colorID);
                return m_fDefaultValue;
            );
            PROP_SET(
                CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject) {
                    if (!obj->m_pDetailColor) continue;
                    if (abs) {
                        obj->m_pDetailColor->colorID = static_cast<int>(f);
                    } else {
                        auto val = static_cast<int>(this->m_fDragCollect / 6.f);
                        obj->m_pDetailColor->colorID = static_cast<int>(this->m_mStartValues[obj]) + val;
                    }
                    if (obj->m_pDetailColor->colorID > 1023)
                        obj->m_pDetailColor->colorID = 1023;
                    if (obj->m_pDetailColor->colorID < 0)
                        obj->m_pDetailColor->colorID = 0;
                }
            );
            PROP_USABLE(
                auto objs = LevelEditorLayer::get()->getEditorUI()->getSelectedObjects();
                CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject) {
                    if (obj->m_pDetailColor)
                        return true;
                }
                return false;
            );
            PROP_VALNAME(
                if (value == 0) return "Default";
                if (value > 999) return GJSpecialColorSelect::textForColorIdx(static_cast<int>(value));
            );
            PROP_NAME("Detail Color");
            this->m_pInput->setAllowedChars(inputf_ColorChannel);
            this->m_bHasAbsoluteModifier = false;
            this->m_fScrollDivisor = 2.f;
        } break;
            
        case kTypeZOrder: {
            PROP_GET(
                if (specific) return static_cast<float>(specific->m_nGameZOrder);
                return static_cast<float>(firstObject(objs)->m_nGameZOrder)
            );
            PROP_SET(
                CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject) {
                    if (abs) {
                        obj->m_nGameZOrder = static_cast<int>(f);
                    } else {
                        auto val = static_cast<int>(this->m_fDragCollect / 6.f);
                        obj->m_nGameZOrder = static_cast<int>(this->m_mStartValues[obj]) + val;
                    }
                    if (obj->m_nGameZOrder > 999)
                        obj->m_nGameZOrder = 999;
                    if (obj->m_nGameZOrder < -999)
                        obj->m_nGameZOrder = -999;
                }
            );
            PROP_USABLE(
                auto objs = LevelEditorLayer::get()->getEditorUI()->getSelectedObjects();
                if (!objs->count()) 
                    return false;
                return true;
            );
            PROP_NAME("Z Order");
            this->m_pInput->setAllowedChars(inputf_NumeralSigned);
            this->m_bHasAbsoluteModifier = false;
            this->m_fScrollDivisor = 2.f;
        } break;

        default: {
            PROP_GET(return 0.f);
            PROP_SET();
            PROP_USABLE(return false;)
            PROP_NAME("Unknown");
        } break;
    }

    this->updateDragDir();

    this->m_fLastDraggedValue = this->getValue();

    return true;
}

void PropContextMenuItem::updateStartValues() {
    auto objs = LevelEditorLayer::get()->getEditorUI()->getSelectedObjects();
    CCARRAY_FOREACH_B_TYPE(objs, obj, GameObject) {
        this->m_mStartValues[obj] = this->m_getValue(objs, obj);
    }
}

bool PropContextMenuItem::mouseDownSuper(MouseButton btn, CCPoint const& pos) {
    if (!this->isUsable())
        return true;
    this->m_fDragCollect = 0.f;
    this->updateStartValues();
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
    this->m_mStartValues.clear();
    this->m_fLastDraggedValue = this->getValue();
    return this->ContextMenuItem::mouseUpSuper(btn, pos);
}

void PropContextMenuItem::mouseDownOutsideSuper(MouseButton, CCPoint const&) {
    this->enableInput(false);
}

float PropContextMenuItem::getValue(GameObject* obj) {
    if (m_getValue) {
        auto objs = LevelEditorLayer::get()->getEditorUI()->getSelectedObjects();
        if (objs->count())
            return m_getValue(objs, obj);
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
    this->m_fDragCollect += val;
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
        auto s = replaceColorChannelNames(str);
        try {
            bool digit = false;
            for (auto const& c : s) {
                if (isdigit(c)) digit = true;
            }
            if (digit) {
                this->setValue(std::stof(s), true);
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
    auto size = this->getScaledContentSize();
    bool image = this->m_drawCube && size.height >= 15.f;
    bool twoLineText = size.width < 60.f && !image;

    if (this->m_bEditingText) {
        this->m_pValueLabel->setString(
            std::string(this->m_pInput->getString() + this->m_sSuffix).c_str()
        );
        this->m_pNameLabel->setString("");
    } else {
        if (twoLineText) {
            this->m_pNameLabel->setString(this->m_getName().c_str());
            std::string text = "";
            if (this->m_specialValueName) text = this->m_specialValueName(this->getValue());
            if (!text.size()) text =
                BetterEdit::formatToString(this->getValue(), 2u) +
                this->m_sSuffix;
            this->m_pValueLabel->setString(text.c_str());
        } else {
            auto name = this->m_getName() + ": ";
            std::string text = "";
            if (this->m_specialValueName) text = this->m_specialValueName(this->getValue());
            if (!text.size()) text =
                BetterEdit::formatToString(this->getValue(), 2u) +
                this->m_sSuffix;
            name += text;
            this->m_pValueLabel->setString(name.c_str());
            this->m_pNameLabel->setString("");
        }
    }

    auto scale = MORD(m_fFontScale, .4f) - (image ? .05f : 0.f);
    this->m_pValueLabel->limitLabelHeight(
        this->getContentSize().height - 3.f,
        scale,
        .01f
    );
    this->m_pValueLabel->limitLabelWidth(
        this->getContentSize().width - 8.f,
        scale,
        .01f
    );
    this->m_pNameLabel->limitLabelHeight(
        this->getContentSize().height - 3.f,
        scale,
        .01f
    );
    this->m_pNameLabel->limitLabelWidth(
        this->getContentSize().width - 8.f,
        scale,
        .01f
    );
    
    if (this->m_usable) {
        this->m_bNoHover = !this->m_usable();
    }

    if (image) {
        this->m_pValueLabel->setPosition(
            this->getContentSize().width / 2,
            6.f
        );
    } else {
        if (twoLineText && !this->m_bEditingText) {
            this->m_pNameLabel->setPosition(
                this->getContentSize() / 2 + 3.5_y
            );
            this->m_pValueLabel->setPosition(
                this->getContentSize() / 2 - 3.5_y
            );
        } else {
            this->m_pValueLabel->setPosition(
                this->getContentSize() / 2
            );
        }
    }

    if (size.height >= 20.f) {
        this->m_pSmallModifierLabel->setAnchorPoint({ 0.f, .5f });
        this->m_pSmallModifierLabel->setPosition(2.f, size.height - 3.f);
        this->m_pSmallModifierLabel->setScale(.12f);
        this->m_pSmallModifierLabel->setVisible(true);
        if (this->smallModifier()) {
            this->m_pSmallModifierLabel->setColor(to3B(m_pMenu->m_colHighlight));
            this->m_pSmallModifierLabel->setOpacity(255);
        } else {
            this->m_pSmallModifierLabel->setColor(to3B(m_pMenu->m_colText));
            this->m_pSmallModifierLabel->setOpacity(90);
        }

        if (this->m_bHasAbsoluteModifier) {
            this->m_pAbsModifierLabel->setAnchorPoint({ 0.f, .5f });
            this->m_pAbsModifierLabel->setPosition(2.f, size.height - 8.f);
            this->m_pAbsModifierLabel->setScale(.12f);
            this->m_pAbsModifierLabel->setVisible(true);
            if (this->absoluteModifier()) {
                this->m_pAbsModifierLabel->setColor(to3B(m_pMenu->m_colHighlight));
                this->m_pAbsModifierLabel->setOpacity(255);
            } else {
                this->m_pAbsModifierLabel->setColor(to3B(m_pMenu->m_colText));
                this->m_pAbsModifierLabel->setOpacity(90);
            }
        } else {
            this->m_pAbsModifierLabel->setVisible(false);
        }
    } else {
        this->m_pSmallModifierLabel->setVisible(false);
        this->m_pAbsModifierLabel->setVisible(false);
    }

    ContextMenuItem::visit();
}

void PropContextMenuItem::draw() {
    if (!this->isUsable()) {
        this->m_pValueLabel->setOpacity(50);
        this->m_pNameLabel->setOpacity(50);
        this->m_pCube->setOpacity(50);
    } else {
        this->m_pValueLabel->setOpacity(255);
        this->m_pNameLabel->setOpacity(255);
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
                pos - size / 2, pos + size / 2, to4f(m_pMenu->m_colText)
            );
            this->m_pValueLabel->setColor(to3B(invert4B(m_pMenu->m_colText)));
        } else {
            auto pos = this->m_pValueLabel->getPosition();
            auto size = this->m_pValueLabel->getScaledContentSize();
            if (size.height < 2.f)
                size.height = 6.f;
            ccDrawSolidRect(
                pos + size / 2,
                pos + CCPoint { size.width / 2 + 0.5f, -size.height / 2 },
                to4f(m_pMenu->m_colText)
            );
            this->m_pValueLabel->setColor(to3B(m_pMenu->m_colText));
        }
    } else {
        this->m_pValueLabel->setColor(to3B(m_pMenu->m_colText));
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

