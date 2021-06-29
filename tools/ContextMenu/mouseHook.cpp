#include "ContextMenu.hpp"
#include <robtop/glfw/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <robtop/glfw/glfw3native.h>
#include <tlhelp32.h>
#include <thread>

using namespace gd;
using namespace gdmake;
using namespace gdmake::extra;
using namespace cocos2d;

CCPoint posToCCPoint(MSLLHOOKSTRUCT* p) {
    return { 0, 0 };
}

void ContextMenu::loadRightClick(HMODULE hMod) {
}

void ContextMenu::unloadRightClick() {
}
