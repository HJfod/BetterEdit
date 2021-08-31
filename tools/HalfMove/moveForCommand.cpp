#include "moveForCommand.hpp"

GDMAKE_HOOK(0x8d890)
CCPoint* __fastcall EditorUI_moveForCommand(EditorUI* self, edx_t edx, CCPoint* pos, EditCommand com) {
    switch (com) {
        case kEditCommandHalfLeft:
            *pos = CCPoint(-15.0f, 0.0f);
            return pos;

        case kEditCommandHalfRight:
            *pos = CCPoint(15.0f, 0.0f);
            return pos;

        case kEditCommandHalfUp:
            *pos = CCPoint(0.0f, 15.0f);
            return pos;

        case kEditCommandHalfDown:
            *pos = CCPoint(0.0f, -15.0f);
            return pos;

        case kEditCommandQuarterLeft:
            *pos = CCPoint(-7.5f, 0.0f);
            return pos;

        case kEditCommandQuarterRight:
            *pos = CCPoint(7.5f, 0.0f);
            return pos;

        case kEditCommandQuarterUp:
            *pos = CCPoint(0.0f, 7.5f);
            return pos;

        case kEditCommandQuarterDown:
            *pos = CCPoint(0.0f, -7.5f);
            return pos;
    }

    return GDMAKE_ORIG_P(self, edx, pos, com);
}
