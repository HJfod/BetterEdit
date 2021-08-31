#pragma once

#include "KeybindManager.hpp"

void loadBEKeybinds();
void moveGameLayerSmooth(EditorUI* ui, CCPoint const& pos);
void focusGameLayerOnObject(EditorUI* ui, GameObject* obj);
void focusGameLayerToSelection(EditorUI* ui);
