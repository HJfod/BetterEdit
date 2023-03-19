#pragma once

#include <Geode/binding/GameObject.hpp>

using namespace geode::prelude;

class ObjectProps {
public:
    enum ObjectID {
        TOGGLE_ORB_ID = 1594,

        COLOR_TRIGGER_ID = 899,
        MOVE_TRIGGER_ID = 901,
        STOP_TRIGGER_ID = 1616,
        PULSE_TRIGGER_ID = 1006,
        ALPHA_TRIGGER_ID = 1007,
        TOGGLE_TRIGGER_ID = 1049,
        SPAWN_TRIGGER_ID = 1268,
        ROTATE_TRIGGER_ID = 1346,
        FOLLOW_TRIGGER_ID = 1347,
        SHAKE_TRIGGER_ID = 1520,
        ANIMATION_TRIGGER_ID = 1585,
        FOLLOW_PLAYER_Y_TRIGGER_ID = 1814,
        TOUCH_TRIGGER_ID = 1595,
        COUNT_TRIGGER_ID = 1611,
        INSTANT_COUNT_TRIGGER_ID = 1811,
        PICKUP_TRIGGER_ID = 1817,
        COLLISION_TRIGGER_ID = 1815,
        ON_DEATH_TRIGGER_ID = 1812,

        COUNTER_ITEM_ID = 1615,
        COLLISION_BLOCK_ID = 1816,
    };

    static bool isRotating(GameObject* obj);
};
