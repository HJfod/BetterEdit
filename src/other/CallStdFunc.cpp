#include <other/CallStdFunc.hpp>

using namespace geode::prelude;
using namespace better_edit;

bool CallStdFunc::init(std::function<void()> func) {
    m_function = func;
    return true;
}

void CallStdFunc::update(float) {
    if (m_function) {
        m_function();
    }
}

CallStdFunc* CallStdFunc::create(std::function<void()> func) {
    auto ret = new CallStdFunc();
    if (ret && ret->init(func)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CCFiniteTimeAction* CallStdFunc::reverse() {
    return CallStdFunc::create(m_function);
}
