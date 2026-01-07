#include "libs/util/functions.h"
#include "libs/util/hooks.hpp"
#include "libs/util/memory.h"
#include "preprocessors.hpp"

__attribute__((naked)) __attribute__((section(".text.main")))
__attribute__((target("arm"))) void
main_entry(void) {
  __asm__ volatile("push {r0-r7, lr}\n");
  CARDi_ReadCard(); // original function call

  // custom overlay modifications:

  // disable sprite pop up in battle
  if (*(u32 *)0x0221fc0c == 0x68609407)
    write_u16((u16 *)0x0221fc0c, 0xe0ed);

  // increase title screen text buffer size from 0x40 to 0x44
  if (*(u32 *)0x021E670C == 0x20401C07)
    write_u8((u8 *)0x021e670e, 0x44);

  if (*(u32 *)0x022530e2 == 0xfc2ff60c)
    SET_HOOK(GET_VEC_SINJOH_RUINS_ORBS);

  if (*(u32 *)0x02252da6 == 0xfdcdf60c)
    SET_HOOK(GET_VEC_SINJOH_RUINS_UNOWN_CIRCLE);

  __asm__ volatile("pop {r0-r7, pc}\n");
}
