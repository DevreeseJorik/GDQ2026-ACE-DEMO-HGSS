#include "libs/util/functions.h"
#include "libs/util/memory.h"

static const char **archiveTable = (const char **)0x0210F210;

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
} CUSTOM_TRAINER_DATA;

typedef struct {
  CUSTOM_TRAINER_DATA editableData;

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

typedef struct {
  u16 dataId;
  CUSTOM_TRAINER_DATA trainerData;
  STRCODE name[8];
  POKEDATA_TYPE_MULTI team[6];
} CUSTOM_TRAINER;

const CUSTOM_TRAINER *customTrainers = (CUSTOM_TRAINER *)0x23C4820;

const CUSTOM_TRAINER *getCustomTrainer(u16 dataId) {
  for (int i = 0; i < 10; i++) {
    const CUSTOM_TRAINER *trainer = &customTrainers[i];
    if (!trainer->dataId)
      break;

    if (trainer->dataId == dataId)
      return trainer;
  }

  return NULL;
}

__attribute__((naked)) __attribute__((section(".text.main")))
__attribute__((target("thumb"))) void
main(void) {
  void *data;
  u32 archiveId, dataId;
  __asm__ volatile("push {r0-r7}\n"
                   "mov %0, r0\n"
                   "mov %1, r1\n"
                   "mov %2, r2\n"
                   : "=r"(data), "=r"(archiveId), "=r"(dataId));

  if (archiveId == 0x37 || archiveId == 0x38) {
    write_u32((u32 *)0x23DFC30, (u32)data);
    write_u32((u32 *)0x23DFC34, (u32)archiveId);
    write_u32((u32 *)0x23DFC38, (u32)dataId);
  }

  ArchiveDataLoadIndex(data, archiveTable[archiveId], dataId, 0, 0);

  switch (archiveId) {
  case 0x37: { // load trainer info
    TRAINER_DATA *trainerData = (TRAINER_DATA *)data;
    memcp((void *)0x23DFD00, trainerData, sizeof(TRAINER_DATA));
    const CUSTOM_TRAINER *customTrainer = getCustomTrainer(dataId);
    if (customTrainer != NULL) {
      memcp((void *)&trainerData->editableData,
            (void *)&customTrainer->trainerData, sizeof(CUSTOM_TRAINER_DATA));
      memcp((void *)&trainerData->name, (void *)&customTrainer->name, 16);
    }

    break;
  }
  case 0x38: { // load team
    POKEDATA_TYPE_MULTI *team = (POKEDATA_TYPE_MULTI *)data;

    memcp((void *)(u8 *)(0x23DFD00 + sizeof(TRAINER_DATA)), team,
          sizeof(POKEDATA_TYPE_MULTI) * 6);

    const CUSTOM_TRAINER *customTrainer = getCustomTrainer(dataId);
    if (customTrainer != NULL) {
      memcp((void *)team, (void *)&customTrainer->team,
            sizeof(POKEDATA_TYPE_MULTI) * 6);
    }
  }
  }

_return:
  __asm__ volatile("pop {r0-r7}\n"
                   "pop {r3, r4, pc}\n");
}
