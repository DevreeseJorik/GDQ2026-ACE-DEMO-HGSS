#include "graphics/poke_sprite.h"

__attribute__((naked)) __attribute__((section(".text.main")))
__attribute__((target("arm"))) void
main(void) {
  __asm__ volatile("bl 0x1ff82c8\n");       // Sys_ReloadMemory
  write_u32((u32 *)0x02000a18, 0xea0f0d80); // uncompressor hook => 0x23C4020
  __asm__ volatile("bl 0x1ff81e8\n");       // Sys_ForceBoot
}