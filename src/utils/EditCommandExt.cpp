#include "EditCommandExt.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <utils/Warn.hpp>

class $modify(EditorUI) {
    BE_ALLOW_START
    BE_ALLOW_FAKE_ENUMS
    CCPoint moveForCommand(EditCommand command) {
        switch (command) {
            case EditCommandExt::QuarterLeft:  return ccp(-7.5f, 0);
            case EditCommandExt::QuarterRight: return ccp(7.5f, 0);
            case EditCommandExt::QuarterUp:    return ccp(0, 7.5f);
            case EditCommandExt::QuarterDown:  return ccp(0, -7.5f);
            
            case EditCommandExt::EighthLeft:   return ccp(-3.75f, 0);
            case EditCommandExt::EighthRight:  return ccp(3.75f, 0);
            case EditCommandExt::EighthUp:     return ccp(0, 3.75f);
            case EditCommandExt::EighthDown:   return ccp(0, -3.75f);

            case EditCommandExt::UnitLeft:     return ccp(-1, 0);
            case EditCommandExt::UnitRight:    return ccp(1, 0);
            case EditCommandExt::UnitUp:       return ccp(0, 1);
            case EditCommandExt::UnitDown:     return ccp(0, -1);

            default: return EditorUI::moveForCommand(command);
        }
    }
    BE_ALLOW_END
};

