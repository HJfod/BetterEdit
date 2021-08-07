#include "repeatPaste.hpp"

static constexpr const int REPEATBTN_TAG = 100;

GameObject* g_pOriginObject = nullptr;
CCArray* g_pOriginObjects = nullptr;
GameObject* g_pPreviousObject = nullptr;
CCArray* g_pPreviousObjects = nullptr;

bool g_bSkipDeselect = false;

#define DUPVAL_FUNC(_getter_, _setter_) \
    target->_setter_(                   \
        target->_getter_() +            \
        previous->_getter_() -          \
        origin->_getter_()              \
    );

#define DUPVAL_VAR(_var_)   \
    target->_var_ =         \
        target->_var_ +     \
        previous->_var_ -   \
        origin->_var_;

#define DUPVAL_VAR_EFFECT(_var_, _min_, _max_)          \
    as<EffectGameObject*>(target)->_var_ =              \
        as<EffectGameObject*>(target)->_var_ +          \
        as<EffectGameObject*>(previous)->_var_ -        \
        as<EffectGameObject*>(origin)->_var_;           \
    if (as<EffectGameObject*>(target)->_var_ < _min_)   \
        as<EffectGameObject*>(target)->_var_ = _min_;   \
    if (as<EffectGameObject*>(target)->_var_ > _max_)   \
        as<EffectGameObject*>(target)->_var_ = _max_;   \

void updateEffectGameObjectLabel(EffectGameObject* target) {
    auto label = as<CCLabelBMFont*>(target->getChildByTag(999));

    if (label) {
        switch (target->m_nObjectID) {
            // instant count
            case 0x713:
                label->setString(
                    CCString::createWithFormat("%i", target->m_nTargetItemID)->getCString()
                );
                break;
            
            case 899:
                label->setString(
                    CCString::createWithFormat("%i", target->m_nTargetColorID)->getCString()
                );
                break;

            default:
                label->setString(
                    CCString::createWithFormat("%i", target->m_nTargetGroupID)->getCString()
                );
        }
    }
}

void repeatDuplicate(GameObject* target, GameObject* previous, GameObject* origin) {
    DUPVAL_FUNC(getPosition, setPosition)
    DUPVAL_FUNC(getRotation, setRotation)
    DUPVAL_FUNC(getScale, updateCustomScale)

    DUPVAL_VAR(m_nGameZOrder)

    DUPVAL_VAR(m_pBaseColor->hue)
    DUPVAL_VAR(m_pBaseColor->saturation)
    DUPVAL_VAR(m_pBaseColor->brightness)

    if (target->m_pDetailColor) {
        DUPVAL_VAR(m_pDetailColor->hue)
        DUPVAL_VAR(m_pDetailColor->saturation)
        DUPVAL_VAR(m_pDetailColor->brightness)
    }

    if (asEffectGameObject(target)) {
        DUPVAL_VAR_EFFECT(m_nTargetGroupID, 0, 999)
        DUPVAL_VAR_EFFECT(m_nTargetColorID, 0, 1012)
        DUPVAL_VAR_EFFECT(m_nCenterGroupID, 0, 999)
        DUPVAL_VAR_EFFECT(m_nTargetCount, -9999, 9999)
        DUPVAL_VAR_EFFECT(m_nTargetItemID, 0, 999)

        updateEffectGameObjectLabel(as<EffectGameObject*>(target));
    }
}

GDMAKE_HOOK(0x87d20)
void __fastcall EditorUI_onDuplicate(EditorUI* self, edx_t edx, CCObject* pSender) {
    if (!g_pPreviousObjects) {
        g_pPreviousObjects = CCArray::create();
        g_pPreviousObjects->retain();
        g_pOriginObjects = CCArray::create();
        g_pOriginObjects->retain();
    }

    auto selBefore = self->m_pSelectedObject;
    auto selBefores = CCArray::create();
    selBefores->retain();
    CCARRAY_FOREACH_B(self->m_pSelectedObjects, obj)
        selBefores->addObject(obj);

    g_bSkipDeselect = true;
    GDMAKE_ORIG_V(self, edx, pSender);
    g_bSkipDeselect = false;

    if (g_pPreviousObject || g_pPreviousObjects->count()) {
        if (!BetterEdit::getRepeatCopyPaste())
            return;

        if (self->m_pSelectedObject)
            repeatDuplicate(self->m_pSelectedObject, g_pPreviousObject, g_pOriginObject);

        if (self->m_pSelectedObjects) {
            CCARRAY_FOREACH_B_BASE(self->m_pSelectedObjects, obj, GameObject*, ix) {
                auto pObj = as<GameObject*>(g_pPreviousObjects->objectAtIndex(ix));
                auto oObj = as<GameObject*>(g_pOriginObjects->objectAtIndex(ix));

                repeatDuplicate(obj, pObj, oObj);
            }
        }
    }

    g_pOriginObject = selBefore;
    g_pOriginObjects->release();
    g_pOriginObjects = selBefores;

    g_pPreviousObject = self->m_pSelectedObject;
    CCARRAY_FOREACH_B(self->m_pSelectedObjects, obj)
        g_pPreviousObjects->addObject(obj);
}

GDMAKE_HOOK(0x86af0)
void __fastcall EditorUI_deselectAll(EditorUI* self) {
    GDMAKE_ORIG_V(self);

    if (g_bSkipDeselect) return;

    g_pPreviousObject = nullptr;
    if (g_pPreviousObjects && g_pPreviousObjects->count())
        g_pPreviousObjects->removeAllObjects();
    g_pOriginObject = nullptr;
    if (g_pOriginObjects && g_pOriginObjects->count())
        g_pOriginObjects->removeAllObjects();
}

void loadPasteRepeatButton(EditorUI* self) {}

void updatePasteRepeatButton(EditorUI* self) {}
