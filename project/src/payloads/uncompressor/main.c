#include "libs/graphics/poke_sprite.h"

__attribute__((naked)) __attribute__((section(".text.main")))
__attribute__((target("arm"))) void
main(void) {
  __asm__ volatile("push {r0-r7, lr}\n");
  // cardi_readcard: overwrites immediate with custom pointer
  write_u32((u32 *)0x020dd8c8, 0x023C4100);

  // archive_data_load
  write_u32((u32 *)0x0200750a, 0xff79f3bc);

  // archive_data_load_malloc
  write_u32_16bit_alligned((u16 *)0x02007526, 0xf96bf3bd);

  // load_narc: bl 0x023c5000, loadSpriteFromNarc
  // write_u32_16bit_alligned((u16 *)0x02009542, 0xfd5df3bb);

  // inject_sprite: bl 0x023c5100, loadOrInjectSprite
  write_u32_16bit_alligned((u16 *)0x0200956A, 0xfdc9f3bb);

  // inject_palette: bl 0x023c5200, loadOrInjectPalette
  write_u32((u32 *)0x0200999c, 0xfc30f3bb);

  __asm__ volatile("pop {r0-r7, pc}\n");
}
