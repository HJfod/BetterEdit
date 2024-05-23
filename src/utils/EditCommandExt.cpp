#include "EditCommandExt.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <utils/Warn.hpp>

class $modify(EditorUI) {
    BE_ALLOW_START
    BE_ALLOW_FAKE_ENUMS
    CCPoint moveForCommand(EditCommand command) {
        auto gridSize = EditorUI::get()->m_gridSize;
        switch (command) {
            case EditCommandExt::QuarterLeft:   return ccp(-1 / 4.f, 0) * gridSize;
            case EditCommandExt::QuarterRight:  return ccp( 1 / 4.f, 0) * gridSize;
            case EditCommandExt::QuarterUp:     return ccp(0,  1 / 4.f) * gridSize;
            case EditCommandExt::QuarterDown:   return ccp(0, -1 / 4.f) * gridSize;

            case EditCommandExt::EighthLeft:   return ccp(-1 / 8.f, 0) * gridSize;
            case EditCommandExt::EighthRight:  return ccp( 1 / 8.f, 0) * gridSize;
            case EditCommandExt::EighthUp:     return ccp(0,  1 / 8.f) * gridSize;
            case EditCommandExt::EighthDown:   return ccp(0, -1 / 8.f) * gridSize;

            case EditCommandExt::UnitLeft:     return ccp(-.1f,  0);
            case EditCommandExt::UnitRight:    return ccp( .1f,  0);
            case EditCommandExt::UnitUp:       return ccp( 0,  .1f);
            case EditCommandExt::UnitDown:     return ccp( 0, -.1f);

            default: return EditorUI::moveForCommand(command);
        }
    }
    BE_ALLOW_END
};

