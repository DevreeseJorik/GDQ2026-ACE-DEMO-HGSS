#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

#include "types.h"
#include <stdint.h>

typedef void NarcSpriteData;
typedef void NarcPaletteData;

#define fp_arm(address, type, param)                                           \
  ((type(*) param)((uintptr_t)(address) & ~1))

#define fp_thumb(address, type, param)                                         \
  ((type(*) param)((uintptr_t)(address) | 1))

#define fp_cardi_readcard fp_arm(0x020dd570, void, (void))
static inline void CARDi_ReadCard(void) { fp_cardi_readcard(); }

#define fp_ArchiveDataLoadIndex                                                \
  fp_thumb(0x0200732c, void, (void *, const char *, int, int, int))
static inline void ArchiveDataLoadIndex(void *data, const char *name, int index,
                                        int ofset, int size) {
  fp_ArchiveDataLoadIndex(data, name, index, ofset, size);
}

#define fp_ArchiveDataLoadIndexMalloc                                          \
  fp_thumb(0x0200740c, void *,                                                 \
           (const char *name, int index, int heap_id, int ofs, int ofs_size,   \
            int flag))

static void *ArchiveDataLoadIndexMalloc(const char *name, int index,
                                        int heap_id, int ofs, int ofs_size,
                                        int flag) {
  return fp_ArchiveDataLoadIndexMalloc(name, index, heap_id, ofs, ofs_size,
                                       flag);
}

#define fp_NNS_G2dGetUnpackedCharacterData                                     \
  fp_arm(0x020b70a8, BOOL, (void *, NarcSpriteData *))

static inline BOOL NNS_G2dGetUnpackedCharacterData(void *dest,
                                                   NarcSpriteData *src) {
  return fp_NNS_G2dGetUnpackedCharacterData(dest, src);
}

#define fp_decodeSprite fp_thumb(0x02009d28, void, (void *, uint32_t))
static inline void decodeSprite(void *spriteBuffer, uint32_t gameId) {
  fp_decodeSprite(spriteBuffer, gameId);
}

#define fp_NNS_G2dGetUnpackedPaletteData                                       \
  fp_arm(0x020b7140, BOOL, (void *, NarcPaletteData *))
static inline BOOL NNS_G2dGetUnpackedPaletteData(void *dest,
                                                 NarcPaletteData *src) {
  return fp_NNS_G2dGetUnpackedPaletteData(dest, src);
}

#endif // _FUNCTIONS_H
