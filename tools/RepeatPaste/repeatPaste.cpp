#include "repeatPaste.hpp"
#include "../AutoSave/autoSave.hpp"

static constexpr const int REPEATBTN_TAG = 100;

CCArray* g_pOrigin = nullptr;
CCArray* g_pPrevious = nullptr;
bool g_bPasting = false;

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

void repeatDuplicate(GameObject* target, GameObject* previous, GameObject* origin) {
    std::cout << origin << " / " << previous << "\n";
    std::cout << origin->getPositionX() << ", " << origin->getPositionY() << " | "
        << previous->getPositionX() << ", " << previous->getPositionY() << "\n";

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

        as<EffectGameObject*>(target)->updateLabel();
    }
}

GDMAKE_HOOK(0x87d20, "_ZN8EditorUI11onDuplicateEPN7cocos2d8CCObjectE")
void __fastcall EditorUI_onDuplicate(EditorUI* self, edx_t edx, CCObject* pSender) {
    auto objs = CCArray::create();
    objs->retain();

    CCARRAY_FOREACH_B(self->getSelectedObjects(), obj)
        objs->addObject(obj);

    GDMAKE_ORIG_V(self, edx, pSender);

    if (!g_bPasting && g_pOrigin) {
        g_pOrigin->removeAllObjects();
        CCARRAY_FOREACH_B(objs, obj)
            g_pOrigin->addObject(obj);
    }

    objs->release();

    if (!g_bPasting && g_pPrevious) {
        g_pPrevious->removeAllObjects();
        CCARRAY_FOREACH_B(self->getSelectedObjects(), obj)
            g_pPrevious->addObject(obj);
    }
}

GDMAKE_HOOK(0x86af0, "_ZN8EditorUI11deselectAllEv")
void __fastcall EditorUI_deselectAll(EditorUI* self) {
    GDMAKE_ORIG_V(self);

    if (g_bPasting) return;

    CATCH_NULL(g_pOrigin)->removeAllObjects();
    CATCH_NULL(g_pPrevious)->removeAllObjects();
}

void repeatPaste(EditorUI* self) {
    g_bPasting = true;
    self->onDuplicate(nullptr);
    g_bPasting = false;

    if (!g_pOrigin || !g_pPrevious) return;
    if (!g_pOrigin->count() || !g_pPrevious->count()) return;

    CCARRAY_FOREACH_B_TYPE(self->getSelectedObjects(), obj, GameObject) {
        repeatDuplicate(
            obj,
            as<GameObject*>(g_pPrevious->objectAtIndex(0)),
            as<GameObject*>(g_pOrigin->objectAtIndex(0))
        );
    }
}

void loadPasteRepeatButton(EditorUI* self) {
    g_pOrigin = CCArray::create();
    g_pOrigin->retain();
    g_pPrevious = CCArray::create();
    g_pPrevious->retain();
}
void updatePasteRepeatButton(EditorUI* self) {}
