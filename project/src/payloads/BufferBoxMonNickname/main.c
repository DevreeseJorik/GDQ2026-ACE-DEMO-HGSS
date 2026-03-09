#include "libs/util/functions.h"
#include "libs/util/memory.h"

BOOL loadNickname(u32 speciesId, STRING *dest) {
  for (int i = 0; i < MAX_MSGS; i++) {
    CUSTOM_MSG_HEADER header = customMsgHeaders[i];
    if (!header.exists)
      return FALSE;

    if (header.type != TYPE_CUSTOM_POKE_NICKNAME)
      continue;

    if (header.dataId != speciesId)
      continue;

    if ((header.dataId == 0x1ED) && (*(u8 *)0x23D5DF0 == 0))
      return FALSE;

    u16 *str = Heap_Alloc(0, header.size);
    if (str) {
      memcp((void *)(u8 *)str, ((u8 *)customMsgHeaders) + header.offset,
            header.size);
      CopyU16ArrayToStringN(dest, str, header.size);
      Heap_Free(str);
    }
    return TRUE;
  }

  return FALSE;
}

__attribute__((naked)) __attribute__((section(".text.main")))
__attribute__((target("thumb"))) void
main() {
  WORDSET *wordset;
  u32 bufId;
  POKEMON_PASO_PARAM *ppp;
  __asm__ volatile("push {r0-r7}\n"
                   "mov %0, r0\n"
                   "mov %1, r1\n"
                   "mov %2, r2\n"
                   : "=r"(wordset), "=r"(bufId), "=r"(ppp));

  u32 speciesId = GetBoxMonData(ppp, 5, wordset->tmpBuf);
  if (!loadNickname(speciesId, wordset->tmpBuf))
    GetBoxMonData(ppp, 119, wordset->tmpBuf);

  SetStringAsPlaceholder(wordset, bufId, wordset->tmpBuf, NULL);
  __asm__ volatile("pop {r0-r7}\n"
                   "pop {r3-r5, pc}");
}
