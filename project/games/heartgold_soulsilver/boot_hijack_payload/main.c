#include "graphics/poke_sprite.h"

__attribute__((naked)) __attribute__((section(".text.main_thumb")))
__attribute__((target("thumb"))) void
main_thumb() {
  set_sprite_hooks();
}

__attribute__((naked)) __attribute__((section(".text.main")))
__attribute__((target("arm"))) void
main(void) {
  __asm__ volatile("bl 0x1ff82c8\n" // Sys_ReloadMemory
                   "push {r0-r7, lr}\n");
  main_thumb();
  __asm__ volatile("pop {r0-r7, pc}\n"
                   "bl 0x1ff81e8\n" // Sys_ForceBoot
  );
}
