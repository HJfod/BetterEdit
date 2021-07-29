#include "../../BetterEdit.hpp"
#include <sstream>

GDMAKE_HOOK(0x793b0)
void __fastcall EditorUI_updateObjectInfoLabel(gd::EditorUI* self) {
    GDMAKE_ORIG_V(self);
    
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto ratio = winSize.width / winSize.height;
    
    if (ratio > 1.6f)
        self->m_pObjectInfoLabel->setPosition(90, 275);
    else
        self->m_pObjectInfoLabel->setPosition(120, 275);

    if (BetterEdit::getDisableExtraObjectInfo())
        return;

    if (self->m_pSelectedObject) {
        auto baseColor = self->m_pSelectedObject->getBaseColor();
        auto detailColor = self->m_pSelectedObject->getDetailColor();
        
        std::stringstream ss;
        ss << self->m_pObjectInfoLabel->getString();

        if (baseColor)
            ss << "HSV: "
                << baseColor->hue << ","
                << baseColor->saturation << (baseColor->saturationChecked ? " (a)" : "") << ","
                << baseColor->brightness << (baseColor->brightnessChecked ? " (a)" : "") << "\n";

        if (detailColor)
            ss << "HSV 2: "
                << detailColor->hue << ","
                << detailColor->saturation << (detailColor->saturationChecked ? " (a)" : "") << ","
                << detailColor->brightness << (detailColor->brightnessChecked ? " (a)" : "") << "\n";
        
        ss << "Rot: " << self->m_pSelectedObject->getRotation() << "\n";
        ss << "Scale: " << self->m_pSelectedObject->getScale() << "\n";
        ss << "Pos: " << self->m_pSelectedObject->getPositionX() << "," << self->m_pSelectedObject->getPositionY() << "\n";
        ss << "ObjID: " << self->m_pSelectedObject->m_nObjectID << "\n";

        ss << "Addr: 0x" << std::hex << reinterpret_cast<uintptr_t>(self->m_pSelectedObject) << std::dec << "\n";

        self->m_pObjectInfoLabel->setString(ss.str().c_str());
    }
}
