#include "libs/util/functions.h"
#include "libs/util/memory.h"

enum { TYPE_NORMAL = 0, TYPE_ARCHIVE };

typedef struct {
  u16 type;
  u16 exists;
  u16 dataId;
  u16 strId;
  u16 size;
  u16 offset;
} CUSTOM_MSG_HEADER;

static const CUSTOM_MSG_HEADER *customMsgHeaders =
    (CUSTOM_MSG_HEADER *)0x023C8000;

#define MAX_MSGS 100

BOOL loadCustomString(const MSGDATA_MANAGER *man, u32 strId, STRBUF *dest) {
  for (int i = 0; i < MAX_MSGS; i++) {
    CUSTOM_MSG_HEADER header = customMsgHeaders[i];
    if (!header.exists)
      return FALSE;

    if (header.type != man->type)
      continue;

    BOOL match = FALSE;
    u32 heapId = 0;
    switch (man->type) {
    case TYPE_NORMAL:
      match = header.strId == strId;
      break;
    case TYPE_ARCHIVE:
      match = header.strId == strId && header.dataId == man->dataId;
      heapId = man->heapId;
      break;
    }

    if (match) {
      STRCODE *str = Heap_AllocAtEnd(heapId, header.size);
      if (str) {
        memcp((void *)(u8 *)str, ((u8 *)customMsgHeaders) + header.offset,
              header.size);
        CopyU16ArrayToStringN(dest, str, header.size);
        Heap_Free(str);
      }
      return TRUE;
    }
  }

  return FALSE;
}

void getStringWrapper(const MSGDATA_MANAGER *man, u32 strId, STRBUF *dest) {
  write_u32((u32 *)0x23DFC10, man->dataId);
  write_u32((u32 *)0x23DFC14, strId);
  write_u32((u32 *)0x23DFC18, man->type);
  write_u32((u32 *)0x23DFC1C, (u32)dest);

  if (loadCustomString(man, strId, dest))
    return;

  switch (man->type) {
  case TYPE_NORMAL:
    ReadMsgData_ExistingTable_ExistingString(man->msgData, strId, dest);
    break;
  case TYPE_ARCHIVE:
    ReadMsgData_ExistingNarc_ExistingString(man->arcHandle, man->dataId, strId,
                                            man->heapId, dest);
    break;
  }
}

__attribute__((naked)) __attribute__((section(".text.main")))
__attribute__((target("thumb"))) void
main(void) {
  __asm__ volatile("push {r0-r7}\n");

  register MSGDATA_MANAGER *man asm("r0");
  register u32 strId asm("r1");
  register STRBUF *dest asm("r2");

  getStringWrapper(man, strId, dest);

  __asm__ volatile("pop {r0-r7}\n"
                   "pop {r3,r4, pc}\n");
}
