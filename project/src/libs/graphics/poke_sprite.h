#ifndef _POKESPRITE_H
#define _POKESPRITE_H

#include "libs/util/functions.h"
#include "libs/util/macros.h"
#include "libs/util/memory.h"
#include "libs/util/types.h"

#define ANIMATION_FRAMES 10
typedef u16 RGBA5_PIXEL;

typedef struct {
  RGBA5_PIXEL pixels[3200]; // 160 x 80, two frames
} PIXEL_BUFFER;

typedef struct {
  u16 palette[16];
} PALETTE;

typedef struct {
  u16 archive_id;
  u16 sprite_id;
  PIXEL_BUFFER pixelBuffer;
  PALETTE palette;
} CUSTOM_POKEMON;

typedef struct {
  u16 archive_id;
  u16 sprite_id;
  u16 heap_id;
} SPRITE_ARCHIVE_INFO;

#define CUSTOM_POKEMON_COUNT 12
typedef struct {
  SPRITE_ARCHIVE_INFO archiveInfo;
  CUSTOM_POKEMON customPokemon[CUSTOM_POKEMON_COUNT];
} CUSTOM_POKEMON_MANAGER;

// align sprites for packaging
static CUSTOM_POKEMON_MANAGER *manager =
    (CUSTOM_POKEMON_MANAGER *)(0x23CB000 - sizeof(SPRITE_ARCHIVE_INFO));

BOOL inline isValidPointer(void *ptr) {
  return ((u32)ptr >= 0x02000000 && (u32)ptr < 0x023E0000);
}

static inline CUSTOM_POKEMON *
getCustomPokemon(SPRITE_ARCHIVE_INFO *archiveInfo) {
  if (!(archiveInfo->archive_id && archiveInfo->sprite_id))
    return NULL;

  for (int i = 0; i < CUSTOM_POKEMON_COUNT; i++) {
    CUSTOM_POKEMON *customPokemon = &manager->customPokemon[i];
    if (customPokemon->archive_id == archiveInfo->archive_id &&
        customPokemon->sprite_id == archiveInfo->sprite_id) {
      return customPokemon;
    }
  }

  return NULL;
}

static inline void loadSpriteFromNarc() {
  register void *dest asm("r0");
  register NarcSpriteData *src asm("r1");
  NNS_G2dGetUnpackedCharacterData(dest, src);
}

static inline void loadOrInjectSprite() {
  register PIXEL_BUFFER *pixelBuffer asm("r0");
  register uint32_t gameId asm("r1");

  CUSTOM_POKEMON *customPokemon = getCustomPokemon(&manager->archiveInfo);
  if (customPokemon != NULL) {
    memcp(pixelBuffer, &customPokemon->pixelBuffer, sizeof(PIXEL_BUFFER));
  } else {
    UnscanPokepic((void *)pixelBuffer, gameId);
  }
}

#endif // _POKESPRITE_H