#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

#include <stdint.h>

#define fp_arm(address, type, param)                                           \
  ((type(*) param)((uintptr_t)(address) & ~1))

#define fp_thumb(address, type, param)                                         \
  ((type(*) param)((uintptr_t)(address) | 1))

#define fp_cardi_readcard fp_arm(0x020dd570, void, (void))
static inline void CARDi_ReadCard(void) { fp_cardi_readcard(); }

#endif // _FUNCTIONS_H
