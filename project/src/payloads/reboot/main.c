#include "libs/util/memory.h"
#include "preprocessors.hpp"

__attribute__((naked)) __attribute__((section(".text.main")))
__attribute__((target("arm"))) void
main(void) {
  __asm__ volatile("bl 0x1ff82c8\n"); // Sys_ReloadMemory
  SET_HOOK(UNCOMPRESSOR);
  __asm__ volatile("bl 0x1ff81e8\n"); // Sys_ForceBoot
}