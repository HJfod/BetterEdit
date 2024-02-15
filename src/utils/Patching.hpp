#pragma once

#include <Geode/utils/general.hpp>

using namespace geode::prelude;

struct PatchAddr {
    uintptr_t value;
    explicit PatchAddr();
    explicit PatchAddr(uintptr_t value);
    void* ptr() const;
};

constexpr int MASK_NONE      = 0b00000;
constexpr int MASK_OUTDATED  = 0b00001;
constexpr int MASK_WIN       = 0b00010;
constexpr int MASK_MAC       = 0b00100;
constexpr int MASK_ANDROID32 = 0b01000;
constexpr int MASK_ANDROID64 = 0b10000;

#ifdef GEODE_IS_WINDOWS
    #define PATCH_FACTORY_VER_1_GD_VERSION 22040
    #define PATCH_FACTORY_CURRENT_MASK MASK_WIN
#elif defined(GEODE_IS_MACOS) 
    #define PATCH_FACTORY_VER_1_GD_VERSION 22000
    #define PATCH_FACTORY_CURRENT_MASK MASK_MAC
#elif defined(GEODE_IS_ANDROID)
    #define PATCH_FACTORY_VER_1_GD_VERSION 22050
    #ifdef GEODE_IS_ANDROID64
        #define PATCH_FACTORY_CURRENT_MASK MASK_ANDROID64
    #else
        #define PATCH_FACTORY_CURRENT_MASK MASK_ANDROID32
    #endif
#else
    static_assert(false, "PatchFactory is not supported on this platform!");
#endif

#if GEODE_COMP_GD_VERSION != PATCH_FACTORY_VER_1_GD_VERSION
    #define PATCH_FACTORY_VER_1_MASK MASK_OUTDATED
#else
    #define PATCH_FACTORY_VER_1_MASK MASK_NONE
#endif

template <int Mask>
class PatchFactory final {
private:
    std::pair<uintptr_t, ByteVector> m_data;
    
    PatchFactory() = default;
    PatchFactory(PatchFactory const&) = delete;
    PatchFactory& operator=(PatchFactory const&) = delete;

    template <int From>
    friend class PatchFactory;

    template <int From>
    PatchFactory(PatchFactory<From>&& from) {
        if constexpr (!(From & MASK_OUTDATED)) {
            m_data = std::move(from.m_data);
        }
    }

    void set(uintptr_t addr, ByteVector const& data) {
        if constexpr (!(Mask & MASK_OUTDATED)) {
            m_data = { addr, data };
        }
    }

public:
    friend PatchFactory<PATCH_FACTORY_VER_1_MASK> createPatches1();

    auto win(uintptr_t offset, ByteVector const& data = {}, bool cocosOffset = false) && {
    #ifdef GEODE_IS_WINDOWS
        this->set((cocosOffset ? base::getCocos() : base::get()) + offset, data);
    #endif
        return PatchFactory<Mask | MASK_WIN>(std::move(*this));
    }
    auto mac(uintptr_t offset, ByteVector const& data = {}) && {
    #ifdef GEODE_IS_MACOS
        this->set(base::get() + offset, data);
    #endif
        return PatchFactory<Mask | MASK_MAC>(std::move(*this));
    }
    auto android32(uintptr_t offset, ByteVector const& data = {}) && {
    #ifdef GEODE_IS_ANDROID32
        this->set(base::get() + offset, data);
    #endif
        return PatchFactory<Mask | MASK_ANDROID32>(std::move(*this));
    }
    auto android64(uintptr_t offset, ByteVector const& data = {}) && {
    #ifdef GEODE_IS_ANDROID64
        this->set(base::get() + offset, data);
    #endif
        return PatchFactory<Mask | MASK_ANDROID64>(std::move(*this));
    }

    PatchAddr addr() {
        static_assert(!(Mask & MASK_OUTDATED), "This patch is outdated!");
        static_assert(
            Mask & PATCH_FACTORY_CURRENT_MASK,
            "This patch is not available on this platform, either add it or #ifdef the code out!"
        );
        return PatchAddr(m_data.first);
    }

    Patch* create() {
        static_assert(!(Mask & MASK_OUTDATED), "This patch is outdated!");
        static_assert(
            Mask & PATCH_FACTORY_CURRENT_MASK,
            "This patch is not available on this platform, either add it or #ifdef the code out!"
        );
        return Mod::get()->patch(m_data.first, m_data.second).unwrap();
    }
};

PatchFactory<PATCH_FACTORY_VER_1_MASK> createPatches1();
