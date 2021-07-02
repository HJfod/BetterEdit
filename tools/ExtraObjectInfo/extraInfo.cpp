#include "../../BetterEdit.hpp"
#include <sstream>

GDMAKE_HOOK(0x793b0)
void __fastcall EditorUI_updateObjectInfoLabel(gd::EditorUI* self) {
    GDMAKE_ORIG_V(self);

    self->m_pObjectInfoLabel->setPositionY(280);

    if (self->m_pSelectedObject) {
        auto baseHSV = self->m_pSelectedObject->getBaseColor();
        auto detailHSV = self->m_pSelectedObject->getDetailColor();

        std::stringstream ss;
        ss << self->m_pObjectInfoLabel->getString();

        ss << "HSV: "
            << baseHSV->hue << ","
            << baseHSV->saturation << (baseHSV->saturationChecked ? " (a)" : "") << ","
            << baseHSV->brightness << (baseHSV->brightnessChecked ? " (a)" : "") << "\n";

        if (detailHSV)
            ss << "HSV 2: "
                << detailHSV->hue << ","
                << detailHSV->saturation << (detailHSV->saturationChecked ? " (a)" : "") << ","
                << detailHSV->brightness << (detailHSV->brightnessChecked ? " (a)" : "") << "\n";
        
        ss << "Rot: " << self->m_pSelectedObject->getRotation() << "\n";
        ss << "Scale: " << self->m_pSelectedObject->getScale() << "\n";
        ss << "Pos: " << self->m_pSelectedObject->getPositionX() << "," << self->m_pSelectedObject->getPositionY() << "\n";
        ss << "ObjID: " << self->m_pSelectedObject->m_nObjectID << "\n";

        ss << "Addr: 0x" << std::hex << reinterpret_cast<uintptr_t>(self->m_pSelectedObject) << std::dec << "\n";

        self->m_pObjectInfoLabel->setString(ss.str().c_str());
    }
}
