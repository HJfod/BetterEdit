#pragma once

#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

enum class CustomEditCommand {
// RobTop
    SmallLeft = 1,
    SmallRight = 2,
    SmallUp = 3,
    SmallDown = 4,

    Left = 5,
    Right = 6,
    Up = 7,
    Down = 8,

    BigLeft = 9,
    BigRight = 10,
    BigUp = 11,
    BigDown = 12,

    TinyLeft = 13,
    TinyRight = 14,
    TinyUp = 15,
    TinyDown = 16,

    FlipX = 17,
    FlipY = 18,
    RotateCW = 19,
    RotateCCW = 20,
    RotateCW45 = 21,
    RotateCCW45 = 22,
    RotateFree = 23,
    RotateSnap = 24,

    Scale = 25,

// Custom
    HalfLeft = 0x400,
    HalfRight = 0x401,
    HalfUp = 0x402,
    HalfDown = 0x403,

    QuarterLeft = 0x404,
    QuarterRight = 0x405,
    QuarterUp = 0x406,
    QuarterDown = 0x407,

    EigthLeft = 0x408,
    EigthRight = 0x409,
    EigthUp = 0x40a,
    EigthDown = 0x40b,

    QUnitLeft = 0x40c,
    QUnitRight = 0x40d,
    QUnitUp = 0x40e,
    QUnitDown = 0x40f
};