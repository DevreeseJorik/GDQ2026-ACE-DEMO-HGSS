#include "libs/graphics/poke_sprite.h"

static VecFx32 *customVec =
    (VecFx32 *)0x23d5de4; // set this during script, this is right after
                          // unpacked data

__attribute__((section(".text.main"))) __attribute__((target("thumb"))) void
main(void *fldobj, VecFx32 *vec) {
  vec->x = customVec->x | 0x8000;
  vec->y = customVec->y;
  vec->z = customVec->z | 0x8000;
}
