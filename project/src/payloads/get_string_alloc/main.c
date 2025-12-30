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
    (CUSTOM_MSG_HEADER *)0x023C4E00;

#define MAX_MSGS 100

STRBUF *loadCustomString(const MSGDATA_MANAGER *man, u32 strId) {
  STRBUF *dest = NULL;

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
      STRCODE *str = sys_AllocMemoryLo(heapId, header.size);
      if (str) {
        memcp((void *)(u8 *)str, ((u8 *)customMsgHeaders) + header.offset,
              header.size);
        dest = STRBUF_Create(header.size, heapId);
        if (dest) {
          setMessageLength(dest, str, header.size);
        }
        sys_FreeMemoryEz(str);
      }
    }
  }

  return dest;
}

STRBUF *getStringWrapper(const MSGDATA_MANAGER *man, u32 strId) {
  if (man->dataId == 0x2D9) {
    write_u32((u32 *)0x23C6020, man->dataId);
    write_u32((u32 *)0x23C6024, strId);
    write_u32((u32 *)0x23C6028, man->type);
  }

  STRBUF *dest = loadCustomString(man, strId);
  if (dest != NULL)
    return dest;

  switch (man->type) {
  case TYPE_NORMAL:
    return getStringAlloc(man->msgData, strId, man->heapId);
  case TYPE_ARCHIVE:
    return getStringByHandleAlloc(man->arcHandle, man->dataId, strId,
                                  man->heapId);
  }

  return NULL;
}

__attribute__((naked)) __attribute__((section(".text.main")))
__attribute__((target("thumb"))) STRBUF *
main(void) {
  __asm__ volatile("push {r1-r7}\n");

  register MSGDATA_MANAGER *man asm("r0");
  register u32 strId asm("r1");
  register u32 heapId asm("r2");

  STRBUF *dest = getStringWrapper(man, strId);

  __asm__ volatile("mov r0, %0\n"
                   "pop {r1-r7}\n"
                   "pop {r3, pc}\n"
                   :
                   : "r"(dest));
}
