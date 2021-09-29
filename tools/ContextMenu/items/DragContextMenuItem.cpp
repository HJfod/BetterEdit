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

bool DragContextMenuItem::init(
    ContextMenu* menu,
    const char* spr,
    const char* txt,
    float divisor,
    float min,
    float max,
    DragContextMenuItem::DragCallback cb,
    DragContextMenuItem::DragValue cv
) {
    if (!PropContextMenuItem::init(menu, kTypeNone))
        return false;

    this->m_pDragCallback = cb;
    this->m_pDragValue = cv;
    this->m_sText = txt;
    this->m_fDivisor = divisor;
    this->m_fMinLimit = min;
    this->m_fMaxLimit = max;

    PROP_GET(return this->m_pDragValue());
    PROP_SET(
        if (abs) {
            auto val = f;
            if (val < this->m_fMinLimit)
                val = this->m_fMinLimit;
            if (val > this->m_fMaxLimit)
                val = this->m_fMaxLimit;
            this->m_pDragCallback(this, val);
        } else {
            auto val = this->m_pDragValue() + f / this->m_fDivisor;
            if (val < this->m_fMinLimit)
                val = this->m_fMinLimit;
            if (val > this->m_fMaxLimit)
                val = this->m_fMaxLimit;
            this->m_pDragCallback(this, val);
        }
    );
    PROP_NAME(this->m_sText);
    PROP_USABLE(return true);
    this->m_sSuffix = "x";
    this->m_bOneLineText = true;
    this->m_pInput->setAllowedChars(inputf_NumeralFloat);

    return true;
}

float DragContextMenuItem::getValue(GameObject*) {
    return m_getValue(nullptr, nullptr);
}

void DragContextMenuItem::setValue(float val, bool b) {
    return m_setValue(nullptr, val, b);
}

DragContextMenuItem* DragContextMenuItem::create(
    ContextMenu* menu,
    const char* spr,
    const char* txt,
    float divisor,
    float min,
    float max,
    DragContextMenuItem::DragCallback cb,
    DragContextMenuItem::DragValue cv
) {
    auto ret = new DragContextMenuItem;

    if (ret && ret->init(menu, spr, txt, divisor, min, max, cb, cv)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

