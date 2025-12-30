#include "libs/graphics/poke_sprite.h"
#include "libs/util/memory.h"

__attribute__((naked)) __attribute__((section(".text.main")))
__attribute__((target("arm"))) void
main(void) {
  __asm__ volatile("push {r0-r7, lr}\n");
  // CARDI_ReadCard: overwrites immediate with custom pointer
  write_u32((u32 *)0x020dd8c8, 0x023C4100);

  // ReadWholeNarcMemberByIdPair
  write_u32((u32 *)0x0200750a, 0xff79f3bc);

  // AllocAndReadWholeNarcMemberByIdPair
  write_u32_16bit_alligned((u16 *)0x02007526, 0xffebf3bc);

  // PokepicManager_BufferCharData: bl 0x023c5000
  // write_u32_16bit_alligned((u16 *)0x02009542, 0xfd5df3bb);

  // PokepicManager_BufferCharData_1: bl 0x023c5100
  write_u32_16bit_alligned((u16 *)0x0200956A, 0xfdc9f3bb);

  // PokepicManager_BufferPlttData_0: bl 0x023c5200
  write_u32((u32 *)0x0200999c, 0xfc30f3bb);

  // ReadMsgData_ExistingTable_ExistingString: bl 0x023C4C00
  write_u32((u32 *)0x0200bb6e, 0xf847f3b9);

  // set game version (HeartGold: 7, SoulSilver: 8)
  write_u32((u32 *)0x20f566c, 0x7);

  write_u32((u32 *)0x020273e4, 0xff8cf39c);

  // prevent loading of trainer names
  write_u32((u32 *)0x02073430, 0x0);

  __asm__ volatile("pop {r0-r7, pc}\n");
}
