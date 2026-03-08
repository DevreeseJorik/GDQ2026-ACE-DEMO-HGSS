#include "libs/util/functions.h"
#include "libs/util/memory.h"

static const char **archiveTable = (const char **)0x0210F210;

typedef u16 STRCODE;
typedef u16 PMS_WORD;

enum {
  TEAM_FORMAT_NORMAL = 0,
  TEAM_FORMAT_WAZA,
  TEAM_FORMAT_ITEM,
  TEAM_FORMAT_MULTI
};

typedef struct {
  u16 sentence_type;
  u16 sentence_id;
  PMS_WORD word[2];
} PMS_DATA;

typedef struct {
  u8 data_type;
  u8 tr_type;
  u8 tr_gra;
  u8 poke_count;

  u16 use_item[4];

  u32 aibit;
  u32 fight_type;

  STRCODE name[8];
  PMS_DATA win_word;
  PMS_DATA lose_word;
} TRAINER_DATA;

typedef struct {
  u8 pow;
  u8 para;
  u16 level;
  u16 monsno;
  u16 itemno;
  u16 move[4];
  u16 custom;
} POKEDATA_TYPE_MULTI;

POKEDATA_TYPE_MULTI test_team[6] = {
    {.pow = 1,
     .para = 2,
     .level = 82,
     .monsno = 0x1ED,
     .itemno = 0,
     .move = {0x0, 0x0, 0x0, 0x0},
     .custom = 0x77},
};

__attribute__((naked)) __attribute__((section(".text.main")))
__attribute__((target("thumb"))) void
main(void) {
  __asm__ volatile("push {r0-r7}\n");

  register void *data asm("r0");
  register int archiveId asm("r1");
  register int dataId asm("r2");

  switch (archiveId) {
  case 0x37: // load trainer info
    ReadFromNarcMemberByPathAndId(data, archiveTable[archiveId], dataId, 0, 0);
    TRAINER_DATA *trainerData = (TRAINER_DATA *)data;
    switch (dataId) {
    default:
      trainerData->poke_count = sizeof(test_team) / sizeof(POKEDATA_TYPE_MULTI);
      trainerData->data_type = TEAM_FORMAT_MULTI;
    }
    break;
  case 0x38: // load team
    switch (dataId) {
    default:
      memcp(data, &test_team, sizeof(test_team));
    }
    break;
  default:
    ReadFromNarcMemberByPathAndId(data, archiveTable[archiveId], dataId, 0, 0);
  }

  __asm__ volatile("pop {r0-r7}\n"
                   "pop {r3, r4, pc}\n");
}
