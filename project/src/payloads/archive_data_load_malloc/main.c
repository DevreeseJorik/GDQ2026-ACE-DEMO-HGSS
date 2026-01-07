#include "libs/graphics/poke_sprite.h"
#include "libs/util/functions.h"
#include "libs/util/memory.h"

typedef struct {
  u16 exists;
  u16 scriptId;
  u16 size;
  u16 offset;
} CUSTOM_SCRIPT_HEADER;

static const char **archiveTable = (const char **)0x0210F210;
static SPRITE_ARCHIVE_INFO archiveInfo;

static const CUSTOM_SCRIPT_HEADER *customEventHeaders =
    (CUSTOM_SCRIPT_HEADER *)0x023C4A00;

void *loadCustomScript(u32 scriptId, u32 heapId) {
  void *buf = NULL;
  write_u32((u32 *)0x23DFC00, scriptId);

  for (int i = 0; i < 100; i++) {
    CUSTOM_SCRIPT_HEADER header = customEventHeaders[i];

    if (!header.exists)
      break;

    if (header.scriptId != scriptId)
      continue;

    buf = sys_AllocMemory(heapId, header.size);
    memcp(buf, (void *)(u8 *)customEventHeaders + header.offset, header.size);
    break;
  }

  return buf;
}

__attribute__((naked)) __attribute__((section(".text.main")))
__attribute__((target("thumb"))) void
main(void) {
  __asm__ volatile("push {r1-r7}\n");

  int archiveId, dataId, heapId;
  __asm__ volatile("mov %0, r0\n"
                   "mov %1, r1\n"
                   "mov %2, r2\n"
                   : "=r"(archiveId), "=r"(dataId), "=r"(heapId));

  archiveInfo.archive_id = archiveId;
  archiveInfo.sprite_id = dataId;
  archiveInfo.heap_id = heapId;

  // write_u32((u32 *)0x23DFB00, archiveId);
  // write_u32((u32 *)0x23DFB04, dataId);
  if (getCustomPokemon(&archiveInfo))
    memcp(&manager->archiveInfo, &archiveInfo, sizeof(SPRITE_ARCHIVE_INFO));

  void *buf = NULL;
  switch (archiveId) {
  case 0xC:
    buf = loadCustomScript(dataId, heapId);
  }

  if (buf == NULL)
    buf = ArchiveDataLoadIndexMalloc(archiveTable[archiveId], dataId, heapId, 0,
                                     0, 0);

  __asm__ volatile("mov r0, %0\n"
                   "pop {r1-r7}\n"
                   "pop {r4, pc}\n"
                   :
                   : "r"(buf));
}
