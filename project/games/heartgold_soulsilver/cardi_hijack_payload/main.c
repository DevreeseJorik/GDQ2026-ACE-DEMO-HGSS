#include "graphics/poke_sprite.h"

__attribute__((naked)) __attribute__((section(".text.main")))
__attribute__((target("arm"))) void
main(void) {
  __asm__ volatile(".word 0x08bd8070\n");

  // compiler doesn't recognise ldmiaeq sp!,{r4,r5,r6,pc} => 70  80  bd  08
}