#pragma once

#include "../../BetterEdit.hpp"

static constexpr const int VIEWBUTTONBAR_TAG = 0x234592;

void loadVisibilityTab(EditorUI*);
void updateVisibilityTab(EditorUI*);
void makeVisibilityPatches();
void setHideLDMObjects();
void setHideLDMObjects(bool);
bool shouldHideLDMObject(GameObject*);
void drawVisibilityStuff();
void showVisibilityTab(EditorUI*, bool);
bool shouldHidePortalLine();
