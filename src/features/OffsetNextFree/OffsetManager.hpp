#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class OffsetManager : public CCObject {
protected:
    inline static OffsetManager* s_instance = nullptr;

public:
    size_t m_groupOffset = 0;
    size_t m_colorOffset = 0;
    static OffsetManager* get() {
        if (!s_instance) {
            s_instance = new OffsetManager;
            if (s_instance) {
                return s_instance;
            }
            CC_SAFE_DELETE(s_instance);
            return nullptr;
        }

        return s_instance;
    }
};