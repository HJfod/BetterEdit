#include <Geode/Geode.hpp>

#include <Geode/modify/ObjectToolbox.hpp>
#include <Geode/modify/EditorUI.hpp>

#include "GridSizeDelegate.hpp"

using namespace geode::prelude;

class $modify(ObjectToolbox) {
    float gridNodeSizeForKey(int objID) {
        if (!GridSizeDelegate::get()->useCustomGridSize()) 
            return ObjectToolbox::gridNodeSizeForKey(objID);
            
        return GridSizeDelegate::get()->getGridSize();
    }
};

class $modify(EditorUI) {
    void updateGridNodeSize() {
        if (!GridSizeDelegate::get()->useCustomGridSize()) 
            return EditorUI::updateGridNodeSize();
        
        auto actualMode = m_selectedMode;

        m_selectedMode = 2; // Mode_Create
        EditorUI::updateGridNodeSize();

        m_selectedMode = actualMode;
    }
};