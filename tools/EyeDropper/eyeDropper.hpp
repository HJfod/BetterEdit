#pragma once

#include "../../BetterEdit.hpp"

CCMenuItemSpriteExtra* createEyeDropperButton(CCNode*);
void loadEyeDropper(ColorSelectPopup* popup);
void loadEyeDropper(SetupPulsePopup* popup);
void showEyeDropper(ColorSelectPopup*);
void showEyeDropper(SetupPulsePopup*);
bool isPickingEyeDropperColor();
bool isLeftMouseButtonPressed();
