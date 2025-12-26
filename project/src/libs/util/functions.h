#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

#include <stdint.h>

#define fp_arm(address, type, param)                                           \
  ((type(*) param)((uintptr_t)(address) & ~1))

#define fp_thumb(address, type, param)                                         \
  ((type(*) param)((uintptr_t)(address) | 1))

#define fp_cardi_readcard fp_arm(0x020dd570, void, (void))
static inline void CARDi_ReadCard(void) { fp_cardi_readcard(); }

#define fp_archive_data_load_index                                             \
  fp_thumb(0x0200732c, void, (void *, const char *, int, int, int))
static inline void ArchiveDataLoadIndex(void *data, const char *name, int index,
                                        int ofset, int size) {
  fp_archive_data_load_index(data, name, index, ofset, size);
}

#endif // _FUNCTIONS_H
