#include "libs/util/functions.h"
#include "libs/util/hooks.hpp"
#include "libs/util/memory.h"

__attribute__((naked)) __attribute__((section(".text.main")))
__attribute__((target("arm"))) void
main_entry(void) {
  __asm__ volatile("push {r0-r7, lr}\n");
  CARDi_ReadCard(); // original function call

  // custom overlay modifications:

  // disable sprite pop up in battle
  if (*(u32 *)0x0221fde4 == 0xDA002804)
    write_u8((u8 *)0x0221fde4, 0x0);

  __asm__ volatile("pop {r0-r7, pc}\n");
}
