// #include <Geode/modify/EditorUI.hpp>
// #include <Geode/cocos/robtop/glfw/glfw3.h>

// using namespace geode::prelude;

// #ifdef GEODE_IS_WINDOWS
// #include <Geode/modify/CCEGLView.hpp>

// static HWND getWindowHandle() {
//     static HWND WINDOW_HANDLE = nullptr;
//     if (WINDOW_HANDLE) return WINDOW_HANDLE;
//     auto window = CCEGLView::get()->getWindow();
//     WINDOW_HANDLE = WindowFromDC(*reinterpret_cast<HDC*>(reinterpret_cast<uintptr_t>(window) + 0x278));
//     return WINDOW_HANDLE;
// }

// static bool MOUSE_WRAP_ENABLED = false;
// static CCPoint MOUSE_WRAP_MOVE_WINDOW = CCPointZero;

// void (__fastcall* _glfwSetCursorPosWin32)(GLFWwindow* window, double x, double y);
// $execute {
//     _glfwSetCursorPosWin32 = reinterpret_cast<decltype(_glfwSetCursorPosWin32)>(base::getCocos() + 0xd4560);
// }

// class $modify(CCEGLView) {
//     $override
//     void onGLFWMouseMoveCallBack(GLFWwindow* window, double x, double y) {
//         if (!MOUSE_WRAP_ENABLED) {
//             return CCEGLView::onGLFWMouseMoveCallBack(window, x, y);
//         }
//         MOUSE_WRAP_ENABLED = false;
//         auto hwnd = getWindowHandle();
//         auto winSize = CCDirector::get()->getWinSize();
//         RECT rect;
//         GetClientRect(hwnd, &rect);
//         SIZE hwndWinSize {
//             .cx = rect.right - rect.left - 1,
//             .cy = rect.bottom - rect.top - 1,
//         };

//         log::info("{}, {}", x, y);

//         bool changed = false;
//         if (x >= hwndWinSize.cx) {
//             x -= hwndWinSize.cx;
//             MOUSE_WRAP_MOVE_WINDOW.x -= winSize.width;
//             changed = true;
//         }
//         else if (x <= 0) {
//             x += hwndWinSize.cx;
//             MOUSE_WRAP_MOVE_WINDOW.x += winSize.width;
//             changed = true;
//         }
//         if (y >= hwndWinSize.cy) {
//             y -= hwndWinSize.cy;
//             MOUSE_WRAP_MOVE_WINDOW.y -= winSize.height;
//             changed = true;
//         }
//         else if (y <= 0) {
//             y += hwndWinSize.cy;
//             MOUSE_WRAP_MOVE_WINDOW.y += winSize.height;
//             changed = true;
//         }
//         if (changed) {
//             _glfwSetCursorPosWin32(CCEGLView::get()->getWindow(), x, y);
//             log::info("changed to {}, {}", x, y);
//         }
//         CCEGLView::onGLFWMouseMoveCallBack(window, x, y);
//     }
// };

// class $modify(EditorUI) {
//     $override
//     void ccTouchMoved(CCTouch* touch, CCEvent* event) {
//         EditorUI::ccTouchMoved(touch, event);
//         MOUSE_WRAP_ENABLED = m_isDraggingCamera;
//         if (m_isDraggingCamera && MOUSE_WRAP_MOVE_WINDOW != CCPointZero) {
//             m_cameraTest += MOUSE_WRAP_MOVE_WINDOW;
//             MOUSE_WRAP_MOVE_WINDOW = CCPointZero;
//         }
//     }
// };

// #endif
