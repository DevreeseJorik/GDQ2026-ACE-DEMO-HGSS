#include "libs/graphics/poke_sprite.h"
#include "preprocessors.hpp"

__attribute__((naked)) __attribute__((section(".text.main")))
__attribute__((target("arm"))) void
main(void) {
  __asm__ volatile("push {r0-r7, lr}\n");
  SET_HOOK(CARDI_READCARD);

  SET_HOOK(ARCHIVE_DATA_LOAD);

  SET_HOOK(ARCHIVE_DATA_LOAD_MALLOC);

  // SET_HOOK(LOAD_NARC);

  SET_HOOK(INJECT_SPRITE);

  SET_HOOK(INJECT_PALETTE);

  SET_HOOK(GET_STRING);

  // SET_HOOK(GET_STRING_ALLOC);

  SET_HOOK(GAME_VERSION);

  SET_HOOK(SAVEDATA_LOAD);

  // prevent loading of trainer names
  write_u32((u32 *)0x02073430, 0x0);

  __asm__ volatile("pop {r0-r7, pc}\n");
}
