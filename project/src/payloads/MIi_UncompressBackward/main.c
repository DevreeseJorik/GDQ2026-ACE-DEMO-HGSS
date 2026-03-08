#include "libs/graphics/poke_sprite.h"
#include "libs/util/memory.h"

__attribute__((naked)) __attribute__((section(".text.main")))
__attribute__((target("arm"))) void
main(void) {
  __asm__ volatile("push {r0-r7, lr}\n");
  write_u32((u32 *)0x020dd8c8, 0x023C4100);
  set_sprite_hooks();
  write_u32((u32 *)0x0200750a, 0xff79f3bc);
  __asm__ volatile("pop {r0-r7, pc}\n");
}
