#pragma once

#include <string>
#include <Geode/Enums.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/binding/GameObject.hpp>

using namespace geode::prelude;

std::string zLayerToString(ZLayer z);

CCArrayExt<GameObject> iterTargets(GameObject* target, CCArray* targets);
CCArrayExt<GameObject> iterSelected(EditorUI* ui);
bool polygonIntersect(std::vector<CCPoint> const& a, CCPoint const& b);
CCArray* selectStructure(EditorUI* ui, GameObject* from);
CCArray* selectStructure(EditorUI* ui, CCArray* from);
void tintObject(GameObject* obj, std::optional<ccColor3B> const& color);
ButtonSprite* createEditorButtonSprite(const char* top, const char* bg = "GJ_button_01.png");
