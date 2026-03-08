#include "util/functions.h"
#include "util/memory.h"

void write_if_matching8(u8 *address, u32 match, u8 value) {
  if (read_u32_unaligned((u32 *)address) == match) {
    write_u8(address, value);
  }
}

void write_if_matching16(u16 *address, u32 match, u16 value) {
  if (read_u32_unaligned((u32 *)address) == match) {
    write_u16(address, value);
  }
}

void write_if_matching32(u32 *address, u32 match, u32 value) {
  if (read_u32_unaligned((u32 *)address) == match) {
    write_u32(address, value);
  }
}

__attribute__((naked)) __attribute__((section(".text.main")))
__attribute__((target("arm"))) void
main(void) {
  __asm__ volatile("push {r0-r7, lr}\n");
  CARDi_ReadCard(); // original function call

  // custom overlay modifications:

  // disable sprite pop up in battle
  write_if_matching8((u8 *)0x0221fde4, 0xDA002804, 0x0);

  __asm__ volatile("pop {r0-r7, pc}\n");
}
