#include "graphics/poke_sprite.h"

__attribute__((naked)) __attribute__((section(".text.main")))
__attribute__((target("arm"))) void
main(void) {
  // __asm__ volatile("bx lr\n");
  __asm__ volatile("push {r0-r7, lr}\n");
  write_u32((u32 *)0x020dd638, 0xea0b9ab0); // cardi_hijack_hook => 0x23C4100
  __asm__ volatile("pop {r0-r7, pc}\n");
}