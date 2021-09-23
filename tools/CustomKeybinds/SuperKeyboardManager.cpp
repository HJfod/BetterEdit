#include "SuperKeyboardManager.hpp"
#include <algorithm>

SuperKeyboardManager* g_manager;

bool SuperKeyboardDelegate::keyDownSuper(enumKeyCodes) { return false; }
bool SuperKeyboardDelegate::keyUpSuper(enumKeyCodes) { return false; }
void SuperKeyboardDelegate::superKeyPushSelf() {
    g_manager->pushDelegate(this);
}
void SuperKeyboardDelegate::superKeyPopSelf() {
    g_manager->popDelegate(this);
}

SuperKeyboardDelegate::SuperKeyboardDelegate() {
    SuperKeyboardManager::get()->pushDelegate(this);
}

SuperKeyboardDelegate::~SuperKeyboardDelegate() {
    SuperKeyboardManager::get()->popDelegate(this);
}

bool SuperKeyboardManager::init() {
    return true;
}

bool SuperKeyboardManager::initGlobal() {
    if (!g_manager) {
        g_manager = new SuperKeyboardManager;
        if (!g_manager || !g_manager->init()) {
            CC_SAFE_DELETE(g_manager);
            return false;
        }
    }
    return true;
}

SuperKeyboardManager* SuperKeyboardManager::get() {
    return g_manager;
}

void SuperKeyboardManager::pushDelegate(SuperKeyboardDelegate* delegate) {
    this->m_vDelegates.insert(this->m_vDelegates.begin(), delegate);
}

void SuperKeyboardManager::popDelegate(SuperKeyboardDelegate* delegate) {
    this->m_vDelegates.erase(std::remove(
        this->m_vDelegates.begin(), this->m_vDelegates.end(), delegate
    ), this->m_vDelegates.end());
}

bool SuperKeyboardManager::dispatchEvent(enumKeyCodes key, bool keydown) {
    if (this->m_vDelegates.size()) {
        if (keydown) {
            if (this->m_vDelegates.at(0)->keyDownSuper(key))
                return true;
        } else {
            if (this->m_vDelegates.at(0)->keyUpSuper(key))
                return true;
        }
    }
    return false;
}
