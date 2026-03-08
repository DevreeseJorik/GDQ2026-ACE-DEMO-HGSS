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

#define fp_ReadFromNarcMemberByPathAndId                                       \
  fp_thumb(0x0200732c, void, (void *, const char *, s32, u32, u32))
static inline void ReadFromNarcMemberByPathAndId(void *dest, const char *path,
                                                 s32 file_idx, u32 offset,
                                                 u32 size) {
  fp_ReadFromNarcMemberByPathAndId(dest, path, file_idx, offset, size);
}

#define fp_AllocAndReadFromNarcMemberByPathAndId                               \
  fp_thumb(0x0200740c, void *, (const char *, s32, u32, u32, u32, BOOL))
static void *AllocAndReadFromNarcMemberByPathAndId(const char *path,
                                                   s32 file_idx, u32 heapID,
                                                   u32 offset, u32 size,
                                                   BOOL allocMode) {
  return fp_AllocAndReadFromNarcMemberByPathAndId(path, file_idx, heapID,
                                                  offset, size, allocMode);
}

#define fp_UnscanPokepic fp_thumb(0x02009d28, void, (void *, u32))
static inline void UnscanPokepic(void *spriteBuffer, u32 gameId) {
  fp_UnscanPokepic(spriteBuffer, gameId);
}

#define fp_ReadMsgData_ExistingTable_ExistingString                            \
  fp_thumb(0x0200b760, void, (void *, u32, STRING *))
static inline void ReadMsgData_ExistingTable_ExistingString(void *msgHeader,
                                                            u32 strId,
                                                            STRING *dest) {
  return fp_ReadMsgData_ExistingTable_ExistingString(msgHeader, strId, dest);
}

#define fp_ReadMsgData_ExistingTable_NewString                                 \
  fp_thumb(0x0200b810, STRING *, (void *, u32, u32))
static inline STRING *
ReadMsgData_ExistingTable_NewString(void *msgHeader, u32 strId, u32 heapId) {
  return fp_ReadMsgData_ExistingTable_NewString(msgHeader, strId, heapId);
}

#define fp_ReadMsgData_ExistingNarc_ExistingString                             \
  fp_thumb(0x0200b8fc, void, (NARC *, u32, u32, u32, STRING *))
static inline void
ReadMsgData_ExistingNarc_ExistingString(NARC *narc, u32 dataId, u32 strId,
                                        u32 heapId, STRING *dest) {
  return fp_ReadMsgData_ExistingNarc_ExistingString(narc, dataId, strId, heapId,
                                                    dest);
}

#define fp_ReadMsgData_ExistingNarc_NewString                                  \
  fp_thumb(0x0200b9f8, STRING *, (NARC *, u32, u32, u32))
static inline STRING *ReadMsgData_ExistingNarc_NewString(NARC *narc, u32 dataId,
                                                         u32 strId,
                                                         u32 heapId) {
  return fp_ReadMsgData_ExistingNarc_NewString(narc, dataId, strId, heapId);
}

#define fp_SetStringAsPlaceholder                                              \
  fp_thumb(0x0200bdf8, void,                                                   \
           (WORDSET *, u32, const STRING *, const WORDSET_PARAM *))
static inline void SetStringAsPlaceholder(WORDSET *wordset, u32 bufId,
                                          const STRING *str,
                                          const WORDSET_PARAM *param) {
  fp_SetStringAsPlaceholder(wordset, bufId, str, param);
}

#define fp_Heap_Alloc fp_thumb(0x0201aa8c, void *, (u32, u32))
static inline void *Heap_Alloc(u32 heapId, u32 size) {
  return fp_Heap_Alloc(heapId, size);
}

#define fp_Heap_AllocAtEnd fp_thumb(0x0201aacc, void *, (u32, u32))
static inline void *Heap_AllocAtEnd(u32 heapId, u32 size) {
  return fp_Heap_AllocAtEnd(heapId, size);
}

#define fp_Heap_Free fp_thumb(0x0201ab0c, void, (void *))
static inline void Heap_Free(void *dest) { fp_Heap_Free(dest); }

#define fp_CopyU16ArrayToStringN                                               \
  fp_thumb(0x02026a04, void, (STRING *, const u16 *, u32))
static inline void CopyU16ArrayToStringN(STRING *str, const u16 *buf,
                                         u32 length) {
  fp_CopyU16ArrayToStringN(str, buf, length);
}

#define fp_String_New fp_thumb(0x02026354, STRING *, (u32, u32))
static inline STRING *String_New(u32 size, u32 heapId) {
  return fp_String_New(size, heapId);
}

#define fp_GetBoxMonData                                                       \
  fp_thumb(0x0206e640, u32, (POKEMON_PASO_PARAM *, int, void *))
static inline u32 GetBoxMonData(POKEMON_PASO_PARAM *ppp, int id, void *buf) {
  return fp_GetBoxMonData(ppp, id, buf);
}

#define fp_NNS_G2dGetUnpackedCharacterData                                     \
  fp_arm(0x020b70a8, BOOL, (void *, NarcSpriteData *))
static inline BOOL NNS_G2dGetUnpackedCharacterData(void *dest,
                                                   NarcSpriteData *src) {
  return fp_NNS_G2dGetUnpackedCharacterData(dest, src);
}

#define fp_NNS_G2dGetUnpackedPaletteData                                       \
  fp_arm(0x020b7140, BOOL, (void *, NarcPaletteData *))
static inline BOOL NNS_G2dGetUnpackedPaletteData(void *dest,
                                                 NarcPaletteData *src) {
  return fp_NNS_G2dGetUnpackedPaletteData(dest, src);
}

#define fp_CARDi_ReadCard fp_arm(0x020dd570, void, (void))
static inline void CARDi_ReadCard(void) { fp_CARDi_ReadCard(); }

static inline void *getBasePointer() { return (void *)*(u32 *)0x021D2228; }

#endif // _FUNCTIONS_H
