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

typedef u16 STRCODE;
typedef u16 PMS_WORD;
typedef struct {
  u16 size;
  u16 strlen;
  u32 magicNumber;

  STRCODE buffer[1];
} STRBUF;

typedef struct {
  u32 offset;
  u32 len;
} MSG_PARAM_BLOCK;

typedef struct {
  u32 numMsgs;
  u32 randValue;
  MSG_PARAM_BLOCK params[];
} MSGDATA_HEADER;

typedef void ARCHANDLE;

typedef struct {
  u16 type;
  u16 heapId;
  u16 archiveId;
  u16 dataId;

  union {
    MSGDATA_HEADER *msgData;
    ARCHANDLE *arcHandle;
  };
} MSGDATA_MANAGER;

#endif // _TYPES_H
