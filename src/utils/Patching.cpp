#include "Patching.hpp"

PatchAddr::PatchAddr() : value(0) {}
PatchAddr::PatchAddr(uintptr_t value) : value(value) {}
void* PatchAddr::ptr() const {
    return reinterpret_cast<void*>(value);
}

PatchFactory<PATCH_FACTORY_VER_1_MASK> createPatches1() {
    return PatchFactory<PATCH_FACTORY_VER_1_MASK>();
}

