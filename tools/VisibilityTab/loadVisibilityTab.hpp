#pragma once

#include "../../BetterEdit.hpp"

void loadVisibilityTab(EditorUI*);
void updateVisibilityTab(EditorUI*);
void makeVisibilityPatches();
void setHideLDMObjects();
void setHideLDMObjects(bool);
bool shouldHideLDMObject(GameObject*);
void drawVisibilityStuff();
void showVisibilityTab(EditorUI*, bool);
bool shouldHidePortalLine();
