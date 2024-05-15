#pragma once

#include <Geode/binding/LevelEditorLayer.hpp>

using namespace geode::prelude;

LevelEditorLayer* createViewOnlyEditor(GJGameLevel* level, utils::MiniFunction<CCLayer*()> returnTo);
bool isViewOnlyEditor(LevelEditorLayer* lel);
