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

#define fp_sys_AllocMemoryLo fp_thumb(0x0201aacc, void *, (u32, u32))
static inline void *sys_AllocMemoryLo(u32 heapId, u32 size) {
  return fp_sys_AllocMemoryLo(heapId, size);
}

#define fp_sys_freeMemoryEz fp_thumb(0x0201ab0c, void, (void *))
static inline void sys_FreeMemoryEz(void *dest) { fp_sys_freeMemoryEz(dest); }

#define fp_getString fp_thumb(0x0200b760, void, (void *, u32, STRBUF *))
static inline void getString(void *msgHeader, u32 strId, STRBUF *dest) {
  return fp_getString(msgHeader, strId, dest);
}

#define fp_getStringAlloc fp_thumb(0x0200b810, STRBUF *, (void *, u32, u32))
static inline STRBUF *getStringAlloc(void *msgHeader, u32 strId, u32 heapId) {
  return fp_getStringAlloc(msgHeader, strId, heapId);
}

#define fp_getStringByHandle                                                   \
  fp_thumb(0x0200b8fc, void, (ARCHANDLE *, u32, u32, u32, STRBUF *))
static inline void getStringByHandle(ARCHANDLE *arcHandle, u32 dataId,
                                     u32 strId, u32 heapId, STRBUF *dest) {
  return fp_getStringByHandle(arcHandle, dataId, strId, heapId, dest);
}

#define fp_getStringByHandleAlloc                                              \
  fp_thumb(0x0200b9f8, STRBUF *, (ARCHANDLE *, u32, u32, u32))
static inline STRBUF *getStringByHandleAlloc(ARCHANDLE *arcHandle, u32 dataId,
                                             u32 strId, u32 heapId) {
  return fp_getStringByHandleAlloc(arcHandle, dataId, strId, heapId);
}

#define fp_setMessageLength                                                    \
  fp_thumb(0x02026a04, void, (STRBUF *, const STRCODE *, u32))
static inline void setMessageLength(STRBUF *strbuf, const STRCODE *str,
                                    u32 size) {
  fp_setMessageLength(strbuf, str, size);
}

#define fp_STRBUF_Create fp_thumb(0x02026354, STRBUF *, (u32, u32))
static inline STRBUF *STRBUF_Create(u32 size, u32 heapId) {
  return fp_STRBUF_Create(size, heapId);
}

static inline void *getBasePointer() { return (void *)*(u32 *)0x021D2228; }

#endif // _FUNCTIONS_H
