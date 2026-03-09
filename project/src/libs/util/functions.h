#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

#include "types.h"
#include <stdint.h>

typedef void NarcSpriteData;
typedef void NarcPaletteData;

/*
 * ROM function declarations
 * Addresses are resolved from the game-specific symbol file at link time.
 * Use long_call attribute to ensure proper range for BL/BLX instructions.
 */

extern void __attribute__((long_call))
ReadFromNarcMemberByPathAndId(void *, const char *, s32, u32, u32);

extern void *__attribute__((long_call))
AllocAndReadFromNarcMemberByPathAndId(const char *, s32, u32, u32, u32, BOOL);

extern void __attribute__((long_call)) UnscanPokepic(void *, u32);

extern void __attribute__((long_call))
ReadMsgData_ExistingTable_ExistingString(void *, u32, STRING *);

extern STRING *__attribute__((long_call))
ReadMsgData_ExistingTable_NewString(void *, u32, u32);

extern void __attribute__((long_call))
ReadMsgData_ExistingNarc_ExistingString(NARC *, u32, u32, u32, STRING *);

extern STRING *__attribute__((long_call))
ReadMsgData_ExistingNarc_NewString(NARC *, u32, u32, u32);

extern void __attribute__((long_call))
SetStringAsPlaceholder(WORDSET *, u32, const STRING *, const WORDSET_PARAM *);

extern void *__attribute__((long_call)) Heap_Alloc(u32, u32);

extern void *__attribute__((long_call)) Heap_AllocAtEnd(u32, u32);

extern void __attribute__((long_call)) Heap_Free(void *);

extern void __attribute__((long_call))
CopyU16ArrayToStringN(STRING *, const u16 *, u32);

extern STRING *__attribute__((long_call)) String_New(u32, u32);

extern u32 __attribute__((long_call))
GetBoxMonData(POKEMON_PASO_PARAM *, int, void *);

extern BOOL __attribute__((long_call))
NNS_G2dGetUnpackedCharacterData(void *, NarcSpriteData *);

extern BOOL __attribute__((long_call))
NNS_G2dGetUnpackedPaletteData(void *, NarcPaletteData *);

extern void __attribute__((long_call)) memcp8(void *, void *, u32);

extern void __attribute__((long_call)) memset(void *, u8, u32);

static inline void memcp(void *dest, void *src, u32 size) {
  memcp8(src, dest, size);
}

extern void __attribute__((long_call)) CARDi_ReadCard(CARDRomStat *p);

static inline void *getBasePointer() { return (void *)*(u32 *)0x021D2228; }

#endif // _FUNCTIONS_H
