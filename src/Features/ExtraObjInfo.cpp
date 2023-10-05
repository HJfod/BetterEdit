#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/GJSpriteColor.hpp>

using namespace geode::prelude;

class $modify(EditorUI) {
    void updateObjectInfoLabel() {
        EditorUI::updateObjectInfoLabel();

        auto winSize = CCDirector::get()->getWinSize();
        auto ratio = winSize.aspect();

        if (ratio > 1.6f) {
            m_objectInfoLabel->setPositionX(90.f);
        }
        else {
            m_objectInfoLabel->setPositionX(120.f);
        }
        
        if (!m_selectedObject) {
            return;
        }

        std::stringstream ss;
        ss << m_objectInfoLabel->getString();

        if (auto col = m_selectedObject->m_baseColor) {
            ss << "HSV: "
                << col->m_hsv.h << ","
                << col->m_hsv.s << (col->m_hsv.absoluteSaturation ? " (a)" : "") << ","
                << col->m_hsv.v << (col->m_hsv.absoluteBrightness ? " (a)" : "") << "\n";
        }

        if (auto col = m_selectedObject->m_detailColor) {
            ss << "HSV 2: "
                << col->m_hsv.h << ","
                << col->m_hsv.s << (col->m_hsv.absoluteSaturation ? " (a)" : "") << ","
                << col->m_hsv.v << (col->m_hsv.absoluteBrightness ? " (a)" : "") << "\n";
        }

        ss << "Rot: " << m_selectedObject->getRotation() << "\n";
        ss << "Scale: " << m_selectedObject->getScale() << "\n";
        ss << "Pos: " << m_selectedObject->getPositionX() << ","
            << m_selectedObject->getPositionY() << "\n";
        ss << "ObjID: " << m_selectedObject->m_objectID << "\n";

        if (Mod::get()->getSettingValue<bool>("developer-mode")) {
            ss << fmt::format("Addr: {}", static_cast<void*>(m_selectedObject)) << "\n";
        }
        
        m_objectInfoLabel->setString(ss.str().c_str());
    }
};
