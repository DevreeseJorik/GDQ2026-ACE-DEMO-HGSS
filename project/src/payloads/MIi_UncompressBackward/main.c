#include "libs/graphics/poke_sprite.h"
#include "preprocessors.hpp"

__attribute__((naked)) __attribute__((section(".text.main")))
__attribute__((target("arm"))) void
main(void) {
  __asm__ volatile("push {r0-r7, lr}\n");
  SET_HOOK(CARDI_READCARD);

  SET_HOOK(READWHOLENARCMEMBERBYIDPAIR);

  SET_HOOK(ALLOCANDREADWHOLENARCMEMBERBYIDPAIR);

  // SET_HOOK(_SUB_POKEPICMANAGER_BUFFERCHARDATA_0);

  SET_HOOK(_SUB_POKEPICMANAGER_BUFFERCHARDATA_1);

  SET_HOOK(_SUB_POKEPICMANAGER_BUFFERPLTTDATA_0);

  SET_HOOK(READMSGDATAINTOSTRING);

  // SET_HOOK(NEWSTRING_READMSGDATA);

  SET_HOOK(GAME_VERSION);

  SET_HOOK(SAVEDATA_TRYLOADONCONTINUE);

  SET_HOOK(BUFFERBOXMONNICKNAME);

  SET_HOOK(GET_BATTLE_BACKGROUND);

  // prevent loading of trainer names
  write_u32((u32 *)0x02073430, 0x0);

  // skip straight to credits (as long as mode of EndGameScreen == 1)
  write_u8((u8 *)0x02052b1a, 0xA);

  __asm__ volatile("pop {r0-r7, pc}\n");
}
