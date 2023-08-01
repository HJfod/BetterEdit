#pragma once

#include "../Macros.hpp"
#include <string>
#include <Geode/Enums.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/binding/GameObject.hpp>

namespace better_edit {
    BE_DLL std::string zLayerToString(ZLayer z);

    BE_DLL geode::cocos::CCArrayExt<GameObject> iterTargets(GameObject* target, cocos2d::CCArray* targets);
    BE_DLL geode::cocos::CCArrayExt<GameObject> iterSelected(EditorUI* ui);

    BE_DLL bool polygonIntersect(std::vector<cocos2d::CCPoint> const& a, cocos2d::CCPoint const& b);

    BE_DLL cocos2d::CCArray* selectStructure(EditorUI* ui, GameObject* from);
    BE_DLL cocos2d::CCArray* selectStructure(EditorUI* ui, cocos2d::CCArray* from);

    BE_DLL void tintObject(GameObject* obj, std::optional<cocos2d::ccColor3B> const& color);
    
    BE_DLL ButtonSprite* createEditorButtonSprite(const char* top, const char* bg = "GJ_button_01.png");
}
