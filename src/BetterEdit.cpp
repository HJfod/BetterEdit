#include "BetterEdit.hpp"
#include <Geode/Loader.hpp>

USE_GEODE_NAMESPACE();

std::unordered_map<uintptr_t, Patch*> PATCHES;

void be::patch(uintptr_t address, byte_array const& bytes, bool apply) {
    if (apply) {
        // apply existing patch
        if (PATCHES.count(address)) {
            PATCHES.at(address)->apply();
        }
        // otherwise create new patch
        else if (auto p = Mod::get()->patch(
            as<void*>(base::get() + address), bytes
        )) {
            PATCHES.insert({ address, p.value() });
        }
    } else {
        if (PATCHES.count(address)) {
            PATCHES.at(address)->restore();
        }
    }
}

void be::nopOut(uintptr_t address, size_t amount, bool apply) {
    be::patch(address, byte_array(amount, 0x90), apply);
}
