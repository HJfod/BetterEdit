#pragma once

#include <Geode/Loader.hpp>

namespace be {
    void patch(uintptr_t address, geode::byte_array const& bytes, bool apply);
    void nopOut(uintptr_t address, size_t amount, bool apply);
}
