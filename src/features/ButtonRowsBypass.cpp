#include <Geode/modify/EditorOptionsLayer.hpp>

using namespace geode::prelude;

class $modify(EditorOptionsLayer) {
    $override
    void onButtonRows(CCObject* sender) {
        if (0) EditorOptionsLayer::onButtonRows(sender);
        
        m_buttonRows = clamp(m_buttonRows + (sender->getTag() ? 1 : -1), 2, 12);
        m_buttonRowsLabel->setString(std::to_string(m_buttonRows).c_str());
    }
    $override
    void onButtonsPerRow(CCObject* sender) {
        if (0) EditorOptionsLayer::onButtonsPerRow(sender);

        m_buttonsPerRow = clamp(m_buttonsPerRow + (sender->getTag() ? 1 : -1), 6, 48);
        m_buttonsPerRowLabel->setString(std::to_string(m_buttonsPerRow).c_str());
    }
};
