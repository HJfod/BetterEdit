#include "MoveForCommand.hpp"

class $modify(EditorUI) {
    CCPoint* moveForCommand(CCPoint* pos, EditCommand com) {
        switch (as<CustomEditCommand>(com)) {
            case CustomEditCommand::HalfLeft:
                *pos = CCPoint(-15.0f, 0.0f);
                return pos;

            case CustomEditCommand::HalfRight:
                *pos = CCPoint(15.0f, 0.0f);
                return pos;

            case CustomEditCommand::HalfUp:
                *pos = CCPoint(0.0f, 15.0f);
                return pos;

            case CustomEditCommand::HalfDown:
                *pos = CCPoint(0.0f, -15.0f);
                return pos;

            case CustomEditCommand::QuarterLeft:
                *pos = CCPoint(-7.5f, 0.0f);
                return pos;

            case CustomEditCommand::QuarterRight:
                *pos = CCPoint(7.5f, 0.0f);
                return pos;

            case CustomEditCommand::QuarterUp:
                *pos = CCPoint(0.0f, 7.5f);
                return pos;

            case CustomEditCommand::QuarterDown:
                *pos = CCPoint(0.0f, -7.5f);
                return pos;

            case CustomEditCommand::EigthLeft:
                *pos = CCPoint(-3.75f, 0.0f);
                return pos;

            case CustomEditCommand::EigthRight:
                *pos = CCPoint(3.75f, 0.0f);
                return pos;

            case CustomEditCommand::EigthUp:
                *pos = CCPoint(0.0f, 3.75f);
                return pos;

            case CustomEditCommand::EigthDown:
                *pos = CCPoint(0.0f, -3.75f);
                return pos;

            case CustomEditCommand::QUnitLeft:
                *pos = CCPoint(-.25f, 0.0f);
                return pos;

            case CustomEditCommand::QUnitRight:
                *pos = CCPoint(.25f, 0.0f);
                return pos;

            case CustomEditCommand::QUnitUp:
                *pos = CCPoint(0.0f, .25f);
                return pos;

            case CustomEditCommand::QUnitDown:
                *pos = CCPoint(0.0f, -.25f);
                return pos;
        }

        return EditorUI::moveForCommand(pos, com);
    }
};