#pragma once

#include "../../BetterEdit.hpp"

void loadVisibilityTab(EditorUI*);
void updateVisibilityTab(EditorUI*);
void makeVisibilityPatches();
bool shouldHideLDMObject(GameObject*);
void drawVisibilityStuff();
void showVisibilityTab(EditorUI*, bool);
