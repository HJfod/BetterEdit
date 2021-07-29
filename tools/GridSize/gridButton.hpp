#pragma once

#include <GDMake.h>

static constexpr const int ZOOMINPUT_TAG = 8003;

cocos2d::CCMenu* getGridButtonParent(gd::EditorUI* self);
void loadGridButtons(gd::EditorUI*);
void showGridButtons(gd::EditorUI*, bool);
