#pragma once

#include <Geode/binding/EditorUI.hpp>

using namespace geode::prelude;

struct EditCommandExt {
    static constexpr auto QuarterLeft  = static_cast<EditCommand>(0x400);
    static constexpr auto QuarterRight = static_cast<EditCommand>(0x401);
    static constexpr auto QuarterUp    = static_cast<EditCommand>(0x402);
    static constexpr auto QuarterDown  = static_cast<EditCommand>(0x403);
    static constexpr auto EighthLeft   = static_cast<EditCommand>(0x404);
    static constexpr auto EighthRight  = static_cast<EditCommand>(0x405);
    static constexpr auto EighthUp     = static_cast<EditCommand>(0x406);
    static constexpr auto EighthDown   = static_cast<EditCommand>(0x407);
};
