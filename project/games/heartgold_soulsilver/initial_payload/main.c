#include "util/memory.h"

void recombine_box_data_chunks(u8 *destAddress) {
  static u32 *baseAddress = (u32 *)0x021D2228;
  static u16 boxDataOffset = 0xF710;

  static u8 pokeCount = 30;
  static u8 boxCount = 18;
  static u8 pokeDataSize = 0x88;

  u32 *basePointer = (u32 *)(*baseAddress);
  u32 *boxDataAddress = (u32 *)(basePointer + boxDataOffset / sizeof(u32));

  for (int i = 0; i < pokeCount * boxCount; i++) {
    u8 *boxDataEntryAddress = (u8 *)boxDataAddress + (i * pokeDataSize);
    memcp(destAddress, boxDataEntryAddress, 0x4);
    memcp(destAddress + 0x4, boxDataEntryAddress + 0x5, pokeDataSize - 0x5);
    destAddress += (pokeDataSize - 0x1);
  }
}

__attribute__((naked)) __attribute__((section(".text.main_thumb")))
__attribute__((target("thumb"))) void
main(void) {
  __asm__ volatile(
      "push {r0-r7, lr}\n"
      "ldr r1, [r0, #0x8]\n"
      "add r1, r1, #0x70\n"
      "str r1, [r0, #0x8]\n"); // advance command pointer to past arbitrary code
  recombine_box_data_chunks((u8 *)0x23C4000);
  write_u32_16bit_alligned((u16 *)0x01ff81d4, 0x2f89eb0f);
  __asm__ volatile("pop {r0-r7, pc}\n");
}
