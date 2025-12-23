#ifndef _POKESPRITE_H
#define _POKESPRITE_H

#include "util/functions.h"
#include "util/macros.h"
#include "util/memory.h"
#include "util/types.h"

#include "sprite_functions.h"

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
  u16 palette_id;
  u16 mode;
  u8 forme_id;
  u8 unknown1[7];
} SPRITE_ARCHIVE_INFO;

typedef struct {
  s16 x;
  s16 y;
  s32 z;

  s16 x_origin;
  s16 y_origin;
  s32 z_origin;

  s16 x2;
  s16 y2;

  u16 x_rotation;
  u16 y_rotation;
  u16 z_rotation;
  u16 id;

  s16 x_rotation2;
  s16 y_rotation2;

  u8 x_offset;
  u8 y_offset;
  u8 x_size;
  u8 y_size;

  u32 unknown_palette;
  u32 color_format;

  u32 red : 5;
  u32 green : 5;
  u32 blue : 5;
  u32 red_overlay : 5;
  u32 g_overlay : 5;
  u32 b_overlay : 5;
  u32 : 2;

  u32 hide : 1;
  u32 show : 1;
  u32 unknown : 7;
  u32 horizontal_flip : 1;
  u32 vertical_flip : 1;
  u32 stop_render : 1;
  u32 fade_effect : 1;
  u32 scramble_effect : 4;
  u32 : 15;
} SPRITE_PARAM;

typedef struct {
  u16 create : 2;
  u16 x : 1;
  u16 y : 1;
  u16 unknown : 1;
  u16 size : 2;
  u16 : 9;
  s8 height;
  u8 unknown2;

  s16 x2;
  s16 y2;

  s16 x_origin;
  s16 y_origin;
} SPRITE_SHADOW;

typedef struct {
  u8 id;
  u8 sequence_id;
  u8 wait;
  u8 unknown1;
  u8 frame[ANIMATION_FRAMES];
  u16 unknown2;
} SPRITE_ANIMATION_INFO;

typedef struct {
  s8 id;
  u8 wait;
} SPRITE_ANIMATION;

typedef struct {
  u32 exists : 1;
  u32 polygon : 6;
  u32 unknown1 : 1;
  u32 unknown2 : 1;
  u32 : 23;

  SPRITE_ARCHIVE_INFO archive_info[2];
  SPRITE_PARAM params;
  SPRITE_ANIMATION_INFO animation_info;
  void *buffer;
  SPRITE_SHADOW shadow[2];
  SPRITE_ANIMATION animations[ANIMATION_FRAMES];
} SPRITE_DATA;

typedef struct {
  BOOL loadSprite;
  SPRITE_DATA *spriteData;
} SPRITE_INFO;

#define CUSTOM_POKEMON_COUNT 12
typedef struct {
  SPRITE_INFO spriteInfo;
  CUSTOM_POKEMON customPokemon[CUSTOM_POKEMON_COUNT];
} CUSTOM_POKEMON_MANAGER;

static CUSTOM_POKEMON_MANAGER *manager = (CUSTOM_POKEMON_MANAGER *)0x23C8000;

#define MAX_SPRITE_ID 4 * 492

static inline void set_sprite_hooks() {
  // function at 0x20094fc

  // bl 0x023c5000, loadSpriteFromNarc
  // + 0x46, see sprite_narc_hook
  write_u32_16bit_alligned((u16 *)0x02009542, 0xfd5df3bb);
  // bl 0x023c5100, loadOrInjectSprite
  // + 0x6A, see sprite_inject_hook
  write_u32_16bit_alligned((u16 *)0x0200956A, 0xfdc9f3bb);

  // function at 0x200994C
  // bl 0x023c5200, loadOrInjectPalette
  // + 0x50, see palette_inject_hook
  write_u32((u32 *)0x0200999c, 0xfc30f3bb);
}

BOOL inline isValidPointer(void *ptr) {
  return ((u32)ptr >= 0x02000000 && (u32)ptr < 0x023E0000);
}

static inline CUSTOM_POKEMON *getCustomPokemon() {
  SPRITE_INFO *spriteInfo = &manager->spriteInfo;
  if (!isValidPointer(spriteInfo->spriteData))
    return NULL;

  SPRITE_ARCHIVE_INFO *archiveInfo = &spriteInfo->spriteData->archive_info[1];

  for (int i = 0; i < 1; i++) {
    CUSTOM_POKEMON *customPokemon = &manager->customPokemon[i];
    // if (customPokemon->archive_id == archiveInfo->archive_id)
    //  &&
    //      customPokemon->sprite_id == archiveInfo->sprite_id) {
    return customPokemon;
    // }
  }

  return NULL;
}

static inline void loadSpriteFromNarc() {
  WITH_CURRENT_REGSTATE(tryLoadSpriteFromNarc, 2);

  register BOOL success asm("r0");
  register SPRITE_DATA *spriteData asm("r6");

  SPRITE_INFO *spriteInfo = &manager->spriteInfo;
  spriteInfo->loadSprite = success;
  spriteInfo->spriteData = spriteData;
}

static inline void loadOrInjectSprite() {
  register PIXEL_BUFFER *pixelBuffer asm("r0");
  register uint32_t gameId asm("r1");

  CUSTOM_POKEMON *customPokemon = getCustomPokemon();
  if (customPokemon != NULL) {
    memcp(pixelBuffer, &customPokemon->pixelBuffer, sizeof(PIXEL_BUFFER));
  } else {
    decodeSprite((void *)pixelBuffer, gameId);
    // memcp(pixelBuffer, &customPokemon->pixelBuffer, sizeof(PIXEL_BUFFER));
  }
}

#endif // _POKESPRITE_H