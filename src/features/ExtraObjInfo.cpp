#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/GJSpriteColor.hpp>

using namespace geode::prelude;

enum SpecialColors {
    Default = 0,
    BG = 1000,
    GROUND = 1001,
    LINE = 1002,
    TDL = 1003,
    OBJ = 1004,
    P1 = 1005,
    P2 = 1006,
    LBG = 1007,
    GROUND_SECOND = 1009,
    BLACK = 1010,
    WHITE = 1011
};

std::string getZLayer(ZLayer layer) {
    switch (layer) {
        case ZLayer::B1:
            return "B1";
        case ZLayer::B2:
            return "B2";
        case ZLayer::B3:
            return "B3";
        case ZLayer::B4:
            return "B4";
        case ZLayer::T1:
            return "T1";
        case ZLayer::T2:
            return "T2";
        case ZLayer::T3:
            return "T3";
        default:
            return "Default";
    }
}

std::string getColorID(GJSpriteColor* color) {
    int colorID;
    if (color->m_colorID == 0) {
        colorID = color->m_defaultColorID;
    } else {
        colorID = color->m_colorID;
    }

    if (colorID < 1000) {
        return std::to_string(color->m_colorID);
    }

    switch (colorID)
    {
        case SpecialColors::BG:
            return "BG";
        case SpecialColors::GROUND:
            return "G1";
        case SpecialColors::LINE:
            return "L";
        case SpecialColors::TDL:
            return "3DL";
        case SpecialColors::OBJ:
            return "OBJ";
        case SpecialColors::P1:
            return "P1";
        case SpecialColors::P2:
            return "P2";
        case SpecialColors::LBG:
            return "LBG";
        case SpecialColors::GROUND_SECOND:
            return "G2";
        case SpecialColors::BLACK:
            return "Black";
        case SpecialColors::WHITE:
            return "White";
    }

    return "?";
}

std::string objectTypeToString(GameObjectType type) {
    switch (type) {
		case GameObjectType::Solid: return "Solid";
		case GameObjectType::Hazard: return "Hazard";
		case GameObjectType::InverseGravityPortal: return "Inverse Gravity Portal";
		case GameObjectType::NormalGravityPortal: return "Normal Gravity Portal";
		case GameObjectType::ShipPortal: return "Ship Portal";
		case GameObjectType::CubePortal: return "Cube Portal";
		case GameObjectType::Decoration: return "Decoration";
		case GameObjectType::YellowJumpPad: return "Yellow Jump Pad";
		case GameObjectType::PinkJumpPad: return "Pink Jump Pad";
		case GameObjectType::GravityPad: return "Gravity Pad";
		case GameObjectType::YellowJumpRing: return "Yellow Jump Orb";
		case GameObjectType::PinkJumpRing: return "Pink Jump Orb";
		case GameObjectType::GravityRing: return "Gravity Orb";
		case GameObjectType::InverseMirrorPortal: return "Inverse Mirror Portal";
		case GameObjectType::NormalMirrorPortal: return "Normal Mirror Portal";
		case GameObjectType::BallPortal: return "Ball Portal";
		case GameObjectType::RegularSizePortal: return "Regular Size Portal";
		case GameObjectType::MiniSizePortal: return "Mini Size Portal";
		case GameObjectType::UfoPortal: return "Ufo Portal";
		case GameObjectType::Modifier: return "Modifier";
		case GameObjectType::SecretCoin: return "Secret Coin";
		case GameObjectType::DualPortal: return "Dual Portal";
		case GameObjectType::SoloPortal: return "Solo Portal";
		case GameObjectType::Slope: return "Slope";
		case GameObjectType::WavePortal: return "Wave Portal";
		case GameObjectType::RobotPortal: return "Robot Portal";
		case GameObjectType::TeleportPortal: return "Teleport Portal";
		case GameObjectType::GreenRing: return "Green Orb";
		case GameObjectType::Collectible: return "Collectible";
		case GameObjectType::UserCoin: return "User Coin";
		case GameObjectType::DropRing: return "Black Orb";
		case GameObjectType::SpiderPortal: return "Spider Portal";
		case GameObjectType::RedJumpPad: return "Red Jump Pad";
		case GameObjectType::RedJumpRing: return "Red Jump Orb";
		case GameObjectType::CustomRing: return "Custom Orb";
		case GameObjectType::DashRing: return "Dash Orb";
		case GameObjectType::GravityDashRing: return "Gravity Dash Orb";
		case GameObjectType::CollisionObject: return "Collision Object";
		case GameObjectType::Special: return "Special";
        default: return "Unknown";
    }
}

class $modify(EditorUI) {
    CCPoint defaultObjectInfoPos;
    CCPoint modifiedObjectInfoPos;
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel)) {
            return false;
        }
        if (!Mod::get()->getSettingValue<bool>("extra-object-info")) {
            return true;
        }
        m_fields->defaultObjectInfoPos = m_objectInfoLabel->getPosition();
        m_objectInfoLabel->setPositionY(m_objectInfoLabel->getPositionY() + 5.f);
        m_objectInfoLabel->setPositionX(90.0f);
        m_fields->modifiedObjectInfoPos = m_objectInfoLabel->getPosition();
        return true;
    }

    void selectObject(GameObject* g, bool filter) {
        EditorUI::selectObject(g, filter);
        this->updateObjectInfoLabel();
    }

    void moveObject(GameObject* g, CCPoint pos) {
        EditorUI::moveObject(g, pos);
        if (
            !Mod::get()->getSettingValue<bool>("extra-object-info") ||
            !Mod::get()->getSettingValue<bool>("dynamic-object-info")
        ) {
            return;
        }
        if (!m_editorLayer->m_editorInitialising) {
            this->updateObjectInfoLabel();
        }
    }

    void ccTouchEnded(CCTouch* touch, CCEvent* event) {
        EditorUI::ccTouchEnded(touch, event);
        if (
            !Mod::get()->getSettingValue<bool>("extra-object-info") || 
            !Mod::get()->getSettingValue<bool>("dynamic-object-info")
        ) {
            return;
        }
        if (m_selectedObject) {
            this->updateObjectInfoLabel();
        }
    }

    void ccTouchMoved(CCTouch* touch, CCEvent* event) {
        EditorUI::ccTouchMoved(touch, event);
        if (
            !Mod::get()->getSettingValue<bool>("extra-object-info") ||
            !Mod::get()->getSettingValue<bool>("dynamic-object-info")
        ) {
            return;
        }
        if (m_selectedObject) {
            this->updateObjectInfoLabel();
        }
    }

    void updateObjectInfoLabel() {
        if (
            !m_objectInfoLabel ||
            !m_selectedObjects ||
            !m_selectedObject ||
            m_selectedObjects->count() > 1 ||
            m_editorLayer->m_editorInitialising
        ) {
            EditorUI::updateObjectInfoLabel();
            return;
        }

        if (!Mod::get()->getSettingValue<bool>("extra-object-info")) {
            m_objectInfoLabel->setPosition(m_fields->defaultObjectInfoPos);
            EditorUI::updateObjectInfoLabel();
            return;
        }
        if (m_objectInfoLabel->getPosition() != m_fields->modifiedObjectInfoPos) {
            m_objectInfoLabel->setPosition(m_fields->modifiedObjectInfoPos);
        }

        std::stringstream ss;
        if (m_selectedObject->m_baseColor && m_selectedObject->m_detailColor) {
            ss << "C1: " << getColorID(m_selectedObject->m_baseColor) << "\n";
            ss << "C2: " << getColorID(m_selectedObject->m_detailColor) << "\n";
        } else {
            ss << "C: " << getColorID(m_selectedObject->m_baseColor) << "\n";
        }
        if (m_selectedObject->getGroupIDCount() > 0) {
            auto groupIDs = m_selectedObject->getGroupIDs();
            std::string str = "";
            ss << "G: ";
            size_t i = 1;
            size_t size = groupIDs.size();
            for (auto group : groupIDs) {
                str += std::to_string(group) + ", ";
                i++;
                if (i == 6) {
                    break;
                }
            }
            str.pop_back();
            str.pop_back();
            if (size >= i) {
                str += "... (" + std::to_string(size - i + 1) + " more)";
            }
            ss << str << "\n";
        }
        if (m_selectedObject->m_zLayer == ZLayer::Default) {
            ss << "Layer: " << getZLayer(m_selectedObject->m_defaultZLayer) << "\n";
        } else {
            ss << "Layer: " << getZLayer(m_selectedObject->m_zLayer) << "\n";
        }
        ss << "Z: " << m_selectedObject->m_gameZOrder << "\n";
        ss << "Editor L: " << m_selectedObject->m_editorLayer << ", " << m_selectedObject->m_editorLayer2 << "\n";

        if (auto col = m_selectedObject->m_baseColor) {
            ss << "HSV: "
                << col->m_hsv.h << ", "
                << col->m_hsv.s << (col->m_hsv.absoluteSaturation ? " (a)" : "") << ", "
                << col->m_hsv.v << (col->m_hsv.absoluteBrightness ? " (a)" : "") << "\n";
        }

        if (auto col = m_selectedObject->m_detailColor) {
            ss << "HSV 2: "
                << col->m_hsv.h << ", "
                << col->m_hsv.s << (col->m_hsv.absoluteSaturation ? " (a)" : "") << ", "
                << col->m_hsv.v << (col->m_hsv.absoluteBrightness ? " (a)" : "") << "\n";
        }

        ss << "Rot: " << m_selectedObject->getRotation() << "\n";
        ss << "Scale: " << m_selectedObject->getScale() << "\n";
        ss << "Pos: " << m_selectedObject->getPositionX() << ", "
            << m_selectedObject->getPositionY() << "\n";
        ss << "ObjID: " << m_selectedObject->m_objectID << "\n";
        ss << "Type: " << objectTypeToString(m_selectedObject->m_objectType) << "\n";

        if (Mod::get()->getSettingValue<bool>("developer-mode")) {
            ss << fmt::format("Addr: {}", static_cast<void*>(m_selectedObject)) << "\n";
        }
        m_objectInfoLabel->setString(ss.str().c_str());
    }
};
