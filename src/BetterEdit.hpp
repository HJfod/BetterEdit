#pragma once

#include <Geode/loader/Hook.hpp>

namespace be {
    void patch(uintptr_t address, geode::ByteVector const& bytes, bool apply);
    void nopOut(uintptr_t address, size_t amount, bool apply);
}
