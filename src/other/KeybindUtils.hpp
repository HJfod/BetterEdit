#pragma once

#include <geode.custom-keybinds/include/Keybinds.hpp>
using namespace keybinds;

#define KEYBIND_FOR_BUTTON(button, keybind) \
    button->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {  \
        if (event->isDown()) {                                                         \
            button->activate();                                                        \
        }                                                                              \
        return ListenerResult::Propagate;                                              \
    }, keybind);                                                                       \