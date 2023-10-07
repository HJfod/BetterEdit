#pragma once

#include "API.hpp"
#include <string>
#include <Geode/Enums.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/binding/GameObject.hpp>

using namespace geode::prelude;

namespace editor_api {
    EDITOR_API_EXPORT std::string zLayerToString(ZLayer z);

    EDITOR_API_EXPORT CCArrayExt<GameObject> iterTargets(GameObject* target, CCArray* targets);
    EDITOR_API_EXPORT CCArrayExt<GameObject> iterSelected(EditorUI* ui);
    EDITOR_API_EXPORT bool polygonIntersect(std::vector<CCPoint> const& a, CCPoint const& b);
    EDITOR_API_EXPORT CCArray* selectStructure(EditorUI* ui, GameObject* from);
    EDITOR_API_EXPORT CCArray* selectStructure(EditorUI* ui, CCArray* from);
    EDITOR_API_EXPORT void tintObject(GameObject* obj, std::optional<ccColor3B> const& color);
    EDITOR_API_EXPORT ButtonSprite* createEditorButtonSprite(const char* top, const char* bg = "GJ_button_01.png");
}
