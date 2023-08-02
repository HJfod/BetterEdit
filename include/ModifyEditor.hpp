#pragma once

#include "Macros.hpp"
#include <Geode/loader/Event.hpp>

namespace better_edit {
    struct BE_DLL GridChangeEvent : public geode::Event {
        float newSize;
        bool locked;
        GridChangeEvent(float size, bool locked);
    };

    class BE_DLL EditorGrid final {
    protected:
        float m_size = 30.f;
        bool m_locked = false;

    public:
        static EditorGrid* get();

        void zoom(bool in);
        float getSize() const;
        void setSize(float size);
        bool isLocked() const;
        void setLocked(bool lock);
    };
}
