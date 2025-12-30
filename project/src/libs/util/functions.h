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

#define fp_CARDi_ReadCard fp_arm(0x020dd570, void, (void))
static inline void CARDi_ReadCard(void) { fp_CARDi_ReadCard(); }

#define fp_ReadFromNarcMemberByPathAndId                                       \
  fp_thumb(0x0200732c, void, (void *, const char *, int, int, int))
static inline void ReadFromNarcMemberByPathAndId(void *data, const char *name,
                                                 int index, int ofset,
                                                 int size) {
  fp_ReadFromNarcMemberByPathAndId(data, name, index, ofset, size);
}

#define fp_AllocAndReadFromNarcMemberByPathAndId                               \
  fp_thumb(0x0200740c, void *,                                                 \
           (const char *name, int index, int heap_id, int ofs, int ofs_size,   \
            int flag))

static void *AllocAndReadFromNarcMemberByPathAndId(const char *name, int index,
                                                   int heap_id, int ofs,
                                                   int ofs_size, int flag) {
  return fp_AllocAndReadFromNarcMemberByPathAndId(name, index, heap_id, ofs,
                                                  ofs_size, flag);
}

#define fp_NNS_G2dGetUnpackedCharacterData                                     \
  fp_arm(0x020b70a8, BOOL, (void *, NarcSpriteData *))

static inline BOOL NNS_G2dGetUnpackedCharacterData(void *dest,
                                                   NarcSpriteData *src) {
  return fp_NNS_G2dGetUnpackedCharacterData(dest, src);
}

#define fp_UnscanPokepic fp_thumb(0x02009d28, void, (void *, uint32_t))
static inline void UnscanPokepic(void *spriteBuffer, uint32_t gameId) {
  fp_UnscanPokepic(spriteBuffer, gameId);
}

#define fp_NNS_G2dGetUnpackedPaletteData                                       \
  fp_arm(0x020b7140, BOOL, (void *, NarcPaletteData *))
static inline BOOL NNS_G2dGetUnpackedPaletteData(void *dest,
                                                 NarcPaletteData *src) {
  return fp_NNS_G2dGetUnpackedPaletteData(dest, src);
}

#define fp_Heap_AllocAtEnd fp_thumb(0x0201aacc, void *, (u32, u32))
static inline void *Heap_AllocAtEnd(u32 heapId, u32 size) {
  return fp_Heap_AllocAtEnd(heapId, size);
}

#define fp_Heap_Free fp_thumb(0x0201ab0c, void, (void *))
static inline void Heap_Free(void *dest) { fp_Heap_Free(dest); }

#define fp_ReadMsgData_ExistingTable_ExistingString                            \
  fp_thumb(0x0200b760, void, (void *, u32, STRBUF *))
static inline void ReadMsgData_ExistingTable_ExistingString(void *msgHeader,
                                                            u32 strId,
                                                            STRBUF *dest) {
  return fp_ReadMsgData_ExistingTable_ExistingString(msgHeader, strId, dest);
}

#define fp_ReadMsgData_ExistingNarc_ExistingString                             \
  fp_thumb(0x0200b8fc, void, (ARCHANDLE *, u32, u32, u32, STRBUF *))
static inline void
ReadMsgData_ExistingNarc_ExistingString(ARCHANDLE *arcHandle, u32 dataId,
                                        u32 strId, u32 heapId, STRBUF *dest) {
  return fp_ReadMsgData_ExistingNarc_ExistingString(arcHandle, dataId, strId,
                                                    heapId, dest);
}

#define fp_CopyU16ArrayToStringN                                               \
  fp_thumb(0x02026a04, void, (STRBUF *, const STRCODE *, u32))
static inline void CopyU16ArrayToStringN(STRBUF *strbuf, const STRCODE *str,
                                         u32 size) {
  fp_CopyU16ArrayToStringN(strbuf, str, size);
}

static inline void *getBasePointer() { return (void *)*(u32 *)0x021D2228; }
#endif // _FUNCTIONS_H
