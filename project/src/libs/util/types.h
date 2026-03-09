#ifndef _TYPES_H
#define _TYPES_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

typedef int BOOL;
typedef u8 S_BOOL;

#define NULL ((void *)0)
#define TRUE (1)
#define FALSE (0)

#define CARD_ROM_PAGE_SIZE 512
typedef struct CARDRomStat {
  void (*read_func)(struct CARDRomStat *);
  u32 ctrl;
  u8 *cache_page;
  u32 dummy[5];
  u8 cache_buf[CARD_ROM_PAGE_SIZE];
} CARDRomStat;

typedef u16 PMS_WORD;
typedef struct {
  u16 size;
  u16 strlen;
  u32 magicNumber;

  u16 buffer[1];
} STRING;

typedef struct {
  u32 offset;
  u32 len;
} MSG_PARAM_BLOCK;

typedef struct {
  u32 numMsgs;
  u32 randValue;
  MSG_PARAM_BLOCK params[];
} MSGDATA_HEADER;

typedef void NARC;

typedef struct {
  u16 type;
  u16 heapId;
  u16 archiveId;
  u16 dataId;

  union {
    MSGDATA_HEADER *msgData;
    NARC *narc;
  };
} MSGDATA_MANAGER;

typedef struct {
  u32 x;
  u32 y;
  u32 z;
} VecFx32;

typedef void POKEMON_PASO_PARAM;

typedef struct {
  u8 def_article;
  u8 indef_article;
  u8 preposition;
  u8 grammer : 7;
  u8 form : 1;
} WORDSET_PARAM;

typedef struct {
  WORDSET_PARAM param;
  STRING *str;
} WORD;

typedef struct {
  u32 max;
  u32 heapID;
  WORD *word;
  STRING *tmpBuf;
} WORDSET;

enum { TYPE_NORMAL = 0, TYPE_ARCHIVE, TYPE_CUSTOM_POKE_NICKNAME };

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

#define ARCHIVE_EVENTDATA 32
#define ARCHIVE_TRAINERDATA 55
#define ARCHIVE_TRAINERTEAM 56

#define MAX_MSGS 100

#endif // _TYPES_H
