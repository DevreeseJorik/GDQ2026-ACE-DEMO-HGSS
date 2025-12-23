#include "util/functions.h"

typedef void NarcSpriteData;
typedef void NarcPaletteData;

#define fp_tryLoadSpriteFromNarc                                               \
  fp_arm(0x020b70a8, BOOL, (void *, NarcSpriteData *))

static inline BOOL tryLoadSpriteFromNarc(void *dest, NarcSpriteData *src) {
  return fp_tryLoadSpriteFromNarc(dest, src);
}

#define fp_decodeSprite fp_thumb(0x02009d28, void, (void *, uint32_t))
static inline void decodeSprite(void *spriteBuffer, uint32_t gameId) {
  fp_decodeSprite(spriteBuffer, gameId);
}

#define fp_tryLoadPaletteFromNarc                                              \
  fp_arm(0x020b7140, BOOL, (void *, NarcPaletteData *))
static inline BOOL tryLoadPaletteFromNarc(void *dest, NarcPaletteData *src) {
  return fp_tryLoadPaletteFromNarc(dest, src);
}
