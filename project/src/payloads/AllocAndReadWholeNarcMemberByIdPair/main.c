#include "libs/graphics/poke_sprite.h"
#include "libs/util/functions.h"
#include "libs/util/memory.h"

static const char **archiveTable = (const char **)0x0210F210;
static SPRITE_ARCHIVE_INFO archiveInfo;

uint16_t customEvent[0x20] = {0x000E, 0x0000, 0x000A, 0x0000, 0x006,
                              0x0000, 0x0002, 0x0000, 0xFD13, 0x0,
                              0x0,    0xA3,   0x0,    0x2};

__attribute__((naked)) __attribute__((section(".text.main")))
__attribute__((target("thumb"))) void
main(void) {
  __asm__ volatile("push {r1-r7}\n");
  register int archiveId asm("r0");
  register int dataId asm("r1");
  register int heapId asm("r2");

  archiveInfo.archive_id = archiveId;
  archiveInfo.sprite_id = dataId;
  archiveInfo.heap_id = heapId;

  if (getCustomPokemon(&archiveInfo))
    memcp(&manager->archiveInfo, &archiveInfo, sizeof(SPRITE_ARCHIVE_INFO));

  void *buf = AllocAndReadFromNarcMemberByPathAndId(archiveTable[archiveId],
                                                    dataId, heapId, 0, 0, 0);

  switch (archiveId) {
  case 0xC: // script data
    switch (dataId) {
    case 0xEB:
      memcp(buf, &customEvent, sizeof(customEvent));
    }
    write_u32((u32 *)0x23C6000, dataId);
    write_u32((u32 *)0x23C6004, (int)buf);
  }

  __asm__ volatile("mov r0, %0\n"
                   "pop {r1-r7}\n"
                   "pop {r4, pc}\n"
                   :
                   : "r"(buf));
}
