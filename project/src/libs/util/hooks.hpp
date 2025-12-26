#pragma once

#ifndef __cplusplus
#error "C++ is required for hooks"
#endif

#include <utility>
#include "libs/util/types.h"

// WARNING: Make sure the function you're hooking into has at least 3 instructions
template <typename Derived>
struct ArmHook {
    template<typename T = Derived>
    using CallbackFuncPtr = decltype(&T::Callback);

    // [3 original instructions]
    // LDR r12, [PC, #0]
    // BX r12
    // .word FUNC_PTR
    alignas(4) inline static struct __attribute__((packed)) {
        u32 bytes[5];
        void* origPtr;
    } data;

    template<typename... Args>
    static __attribute__((always_inline)) decltype(auto) Orig(Args &&... args) {
        return static_cast<CallbackFuncPtr<>>(&data.bytes)(std::forward<Args>(args)...);
    }

    static __attribute__((always_inline)) void Install(void* address) {
        auto instrPtr = static_cast<u32*>(address);
        data.origPtr = reinterpret_cast<void*>(&instrPtr[3]);
        for (int i = 0; i < 3; i++) {
            data.bytes[i] = instrPtr[i];
        }
        data.bytes[3] = 0xe59fc000;
        data.bytes[4] = 0xe12fff1c;

        // ldr r12, [PC, #0]
        // bx r12
        // .word FUNC_PTR
        instrPtr[0] = 0xe59fc000;
        instrPtr[1] = 0xe12fff1c;
        instrPtr[2] = reinterpret_cast<u32>(&Derived::Callback);
    }
};

// WARNING: Make sure the function you're hooking into has at least 16 bytes of instructions and the last one does not land in the middle of a 4-byte instruction
template <typename Derived>
struct ThumbReplace {
    template<typename T = Derived>
    using CallbackFuncPtr = decltype(&T::Callback);

    static __attribute__((always_inline)) void Install(void* address) {
        auto instrPtr = static_cast<u16*>(address);
        // push {r3}
        // ldr r3, [PC, #8]
        // mov r12, r3
        // pop {r3}
        // bx r12
        // .word FUNC_PTR
        instrPtr[0] = 0xb408;
        instrPtr[1] = 0x4b02;
        instrPtr[2] = 0x469c;
        instrPtr[3] = 0xbc08;
        instrPtr[4] = 0x4760;
        instrPtr[5] = 0xbf00;
        *reinterpret_cast<u32*>(&instrPtr[6]) = reinterpret_cast<u32>(&Derived::Callback);
    }
};

// TODO: Replacement hooks?

#define hook_arm(name) struct name : ArmHook<name>
#define replace_thumb(name) struct name : ThumbReplace<name>
