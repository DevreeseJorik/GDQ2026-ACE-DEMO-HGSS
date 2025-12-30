#include "libs/graphics/poke_sprite.h"

__attribute__((naked)) __attribute__((section(".text.main")))
__attribute__((target("thumb"))) void
main(void) {
  __asm__ volatile("push {r0-r7}\n");

  void *basePointer = getBasePointer();
  write_u16((u16 *)(u8 *)(basePointer + 0x237C), 0x125);
  write_u16((u16 *)(u8 *)(basePointer + 0x237E), 0x1B0);
  write_u16((u16 *)(u8 *)(basePointer + 0x2380), 0x08);
  write_u16((u16 *)(u8 *)(basePointer + 0x2382), 0x126);

  __asm__ volatile("pop {r0-r7}\n"
                   "mov r0, #1\n"
                   "pop {r4,pc}");
}
