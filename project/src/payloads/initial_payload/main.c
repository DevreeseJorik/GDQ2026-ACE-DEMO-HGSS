#include "libs/util/memory.h"

void unpack_box_data(u8 *destAddress) {
  static u32 *baseAddress = (u32 *)0x021D2228;
  static u16 boxDataOffset = 0xF710;

  static u8 pokeCount = 30;
  static u8 boxCount = 18;
  static u8 pokeDataSize = 0x88;
  static u8 boxPaddingSize = 0x10;

  u32 *basePointer = (u32 *)(*baseAddress);
  u8 *boxDataAddress = (u8 *)(basePointer) + boxDataOffset;

  u8 *srcAddress = boxDataAddress;
  for (u32 box = 0; box < boxCount; box++) {
    for (u32 poke = 0; poke < pokeCount; poke++) {
      u8 pokeEntry = box * pokeCount + poke;
      memcp(destAddress, srcAddress, 0x4);
      memcp(destAddress + 0x4, srcAddress + 0x5, pokeDataSize - 0x5);
      srcAddress += pokeDataSize;
      destAddress += (pokeDataSize - 0x1);
    }
    srcAddress += boxPaddingSize;
  }
}

__attribute__((naked)) __attribute__((section(".text.main_thumb")))
__attribute__((target("thumb"))) void
main(void) {
  __asm__ volatile(
      "push {r0-r7, lr}\n"
      "ldr r1, [r0, #0x8]\n"
      "add r1, r1, #0xFE\n"
      "str r1, [r0, #0x8]\n"); // advance command pointer to past arbitrary code
  unpack_box_data((u8 *)0x23C4000);
  write_u32((u32 *)0x01ff81d4, 0xeb0f2f89);
  __asm__ volatile("pop {r0-r7, pc}\n");
}
