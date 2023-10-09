#pragma once

#include "API.hpp"
#include <string>
#include <Geode/Enums.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/binding/GameObject.hpp>

using namespace geode::prelude;

namespace editor_api {
    /**
     * Convert a ZLayer into a readable string (like T1 or B3)
     * @param z The ZLayer value to convert (as an instance of the ZLayer enum)
     * @returns The Z layer as a string
     */
    EDITOR_API_EXPORT std::string zLayerToString(ZLayer z);

    /**
     * Helper function to iterate objects for GD
     * @param target The first object to add to the iteration; may be nullptr
     * @param targets The array of objects to iterate; may be nullptr
     * @returns CCArrayExt iterator for the passed objects
     * @note If neither are nullptr, target is first in the iteration list
     */
    EDITOR_API_EXPORT CCArrayExt<GameObject> iterTargets(GameObject* target, CCArray* targets);

    /**
     * Helper function to iterate selected objects
     * @param ui Editor UI
     * @returns CCArrayExt iterator for selected objects
     */
    EDITOR_API_EXPORT CCArrayExt<GameObject> iterSelected(EditorUI* ui);

    /**
     * Check whether a point is within a polygon
     * @warning Currently unimplemented!!
     */
    EDITOR_API_EXPORT bool polygonIntersect(std::vector<CCPoint> const& a, CCPoint const& b);

    /**
     * Smartly expand selection to a whole structure based on an object in it
     * @param ui The editor
     * @param from The object to expand the selection from
     * @returns List of objects to select for the structure
     * @note Actually doesn't select the structure, only returns the objects in the structure
     */
    EDITOR_API_EXPORT CCArray* selectStructure(EditorUI* ui, GameObject* from);

    /**
     * Smartly expand selection to a whole structure based on objects in it
     * @param ui The editor
     * @param from The objects to expand the selection from
     * @returns List of objects to select for the structure
     * @note Actually doesn't select the structure, only returns the objects in the structure
     */
    EDITOR_API_EXPORT CCArray* selectStructure(EditorUI* ui, CCArray* from);

    /**
     * Color an object a different color
     * @param obj The object to tint
     * @param color The color to tint the object to, or nullopt to reset to default
     * @warning Currently unimplemented!!
     */
    EDITOR_API_EXPORT void tintObject(GameObject* obj, std::optional<ccColor3B> const& color);

    /**
     * Helper to create a square button that fits in the editor's UI
     * @param top The top sprite's frame name
     * @param bg The background sprite
     * @returns The created sprite
     */
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
