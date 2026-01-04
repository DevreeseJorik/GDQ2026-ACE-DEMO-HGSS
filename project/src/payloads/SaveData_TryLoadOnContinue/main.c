#include "libs/graphics/poke_sprite.h"

void setLocation(void *basePointer, u16 mapId, u16 x, u16 y, u16 z) {
  write_u16((u16 *)(u8 *)(basePointer + 0x1244), mapId);

  write_u32((u32 *)(u8 *)(basePointer + 0x124C), x);
  write_u32((u32 *)(u8 *)(basePointer + 0x1250), y);
  write_u32((u32 *)(u8 *)(basePointer + 0x1254), z);

  write_u32_16bit_alligned((u16 *)(u8 *)(basePointer + 0x237E), x);
  write_u32_16bit_alligned((u16 *)(u8 *)(basePointer + 0x2382), y);
  write_u32_16bit_alligned((u16 *)(u8 *)(basePointer + 0x2386), z);
}

__attribute__((naked)) __attribute__((section(".text.main")))
__attribute__((target("thumb"))) void
main(void) {
  __asm__ volatile("push {r0-r7}\n");

  void *basePointer = getBasePointer();
  setLocation(basePointer, 0x40, 2, 9, 0);

  write_u32((u32 *)(u8 *)(basePointer + 0x2364), 0x10102);
  write_u32((u32 *)(u8 *)(basePointer + 0x2368), 0xC70001);
  write_u16((u16 *)(u8 *)(basePointer + 0x2378), 0x3);
  write_u16((u16 *)(u8 *)(basePointer + 0x237C), 0x4);

  __asm__ volatile("pop {r0-r7}\n"
                   "mov r0, #1\n"
                   "pop {r4,pc}");
}
