#pragma once

#include "../../BetterEdit.hpp"

void loadEditorKeybindIndicators(EditorUI*);
void clearEditorKeybindIndicators(EditorUI*);
void updateEditorKeybindIndicators();
void showEditorKeybindIndicators(bool);
// lol
void showEditorKeybindIndicatorIfItsTargetIsBeingHovered();
void addIndicator(EditorUI*, CCNode* target, const char* keybind);
