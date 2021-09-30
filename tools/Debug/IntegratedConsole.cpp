#include "IntegratedConsole.hpp"

void IntegratedConsole::setup() {

}

IntegratedConsole* IntegratedConsole::create() {
    auto ret = new IntegratedConsole;

    if (ret && ret->init(400.f, 320.f, "GJ_square05.png", "Integrated Console")) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}
