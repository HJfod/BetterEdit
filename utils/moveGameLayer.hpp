#pragma once

#include "../BetterEdit.hpp"

static constexpr const float w_edge = 120.0f;

void moveGameLayerSmooth(EditorUI* ui, CCPoint const& pos);
void moveGameLayerSmoothAbs(EditorUI* ui, CCPoint const& pos);
void focusGameLayerOnObject(EditorUI* ui, GameObject* obj);
void focusGameLayerToSelection(EditorUI* ui);

