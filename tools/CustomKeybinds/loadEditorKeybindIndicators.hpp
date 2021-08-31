#pragma once

#include "../../BetterEdit.hpp"

void loadEditorKeybindIndicators(EditorUI*);
void clearEditorKeybindIndicators(EditorUI*);
void addIndicator(CCNode* target, const char* keybind);
