#include "../../BetterEdit.hpp"
#include <sstream>

std::string objectTypeToString(GameObjectType type) {
    switch (type) {
		case kGameObjectTypeSolid: return "Solid";
		case kGameObjectTypeHazard: return "Hazard";
		case kGameObjectTypeInverseGravityPortal: return "InverseGravityPortal";
		case kGameObjectTypeNormalGravityPortal: return "NormalGravityPortal";
		case kGameObjectTypeShipPortal: return "ShipPortal";
		case kGameObjectTypeCubePortal: return "CubePortal";
		case kGameObjectTypeDecoration: return "Decoration";
		case kGameObjectTypeYellowJumpPad: return "YellowJumpPad";
		case kGameObjectTypePinkJumpPad: return "PinkJumpPad";
		case kGameObjectTypeGravityPad: return "GravityPad";
		case kGameObjectTypeYellowJumpRing: return "YellowJumpRing";
		case kGameObjectTypePinkJumpRing: return "PinkJumpRing";
		case kGameObjectTypeGravityRing: return "GravityRing";
		case kGameObjectTypeInverseMirrorPortal: return "InverseMirrorPortal";
		case kGameObjectTypeNormalMirrorPortal: return "NormalMirrorPortal";
		case kGameObjectTypeBallPortal: return "BallPortal";
		case kGameObjectTypeRegularSizePortal: return "RegularSizePortal";
		case kGameObjectTypeMiniSizePortal: return "MiniSizePortal";
		case kGameObjectTypeUfoPortal: return "UfoPortal";
		case kGameObjectTypeModifier: return "Modifier";
		case kGameObjectTypeSecretCoin: return "SecretCoin";
		case kGameObjectTypeDualPortal: return "DualPortal";
		case kGameObjectTypeSoloPortal: return "SoloPortal";
		case kGameObjectTypeSlope: return "Slope";
		case kGameObjectTypeWavePortal: return "WavePortal";
		case kGameObjectTypeRobotPortal: return "RobotPortal";
		case kGameObjectTypeTeleportPortal: return "TeleportPortal";
		case kGameObjectTypeGreenRing: return "GreenRing";
		case kGameObjectTypeCollectible: return "Collectible";
		case kGameObjectTypeUserCoin: return "UserCoin";
		case kGameObjectTypeDropRing: return "DropRing";
		case kGameObjectTypeSpiderPortal: return "SpiderPortal";
		case kGameObjectTypeRedJumpPad: return "RedJumpPad";
		case kGameObjectTypeRedJumpRing: return "RedJumpRing";
		case kGameObjectTypeCustomRing: return "CustomRing";
		case kGameObjectTypeDashRing: return "DashRing";
		case kGameObjectTypeGravityDashRing: return "GravityDashRing";
		case kGameObjectTypeCollisionObject: return "CollisionObject";
		case kGameObjectTypeSpecial: return "Special";
        default: return "Unknown";
    }
}

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

        ss << "Type: " << objectTypeToString(self->m_pSelectedObject->m_nObjectType) << "\n";

        self->m_pObjectInfoLabel->setString(ss.str().c_str());
    }
}
