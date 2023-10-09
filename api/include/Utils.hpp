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

    /**
     * Move the object layer in the editor to focus on a point
     * @param ui Editor UI
     * @param pos The position to move to (centered on screen)
     * @param smooth Whether to smoothly animate, or instantly move the camera
     * @param relative Whether to add the position to the current position of 
     * the camera, or treat pos as an absolute position
     */
    EDITOR_API_EXPORT void moveGameLayerTo(EditorUI* ui, CCPoint const& pos, bool smooth = true, bool relative = false);

    /**
     * Move the object layer in the editor to focus on an object
     * @param ui Editor UI
     * @param pos The object to focus on (centered on screen)
     * @param smooth Whether to smoothly animate, or instantly move the camera
     */
    EDITOR_API_EXPORT void moveGameLayerTo(EditorUI* ui, GameObject* obj, bool smooth = true);

    /**
     * Move the object layer in the editor to focus on an object
     * @param ui Editor UI
     * @param objs Array of GameObjects
     * @param smooth Whether to smoothly animate, or instantly move the camera
     */
    EDITOR_API_EXPORT void moveGameLayerTo(EditorUI* ui, CCArray* objs, bool smooth = true);
}
