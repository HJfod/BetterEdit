#pragma once

#include <Geode/DefaultInclude.hpp>

bool objectIsSaw(GameObject* obj);
void beginRotations(LevelEditorLayer* self);
void stopRotations(LevelEditorLayer* self);
void pauseRotations(LevelEditorLayer* self);
void resumeRotations(LevelEditorLayer* self);
void beginRotateSaw(GameObject* obj);
void stopRotateSaw(GameObject* obj);
bool shouldRotateSaw();
void enableRotations(bool);
void onRotateSaws(cocos2d::CCObject*);