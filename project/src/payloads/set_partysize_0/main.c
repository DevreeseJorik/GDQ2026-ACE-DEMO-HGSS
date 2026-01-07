#include "libs/util/functions.h"
#include "libs/util/memory.h"

__attribute__((section(".text.main"))) __attribute__((target("thumb"))) BOOL
main(void *core) {
  void *basePointer = getBasePointer();
  write_u8((u8 *)(basePointer) + 0xA4, 0);
  return 1;
}
