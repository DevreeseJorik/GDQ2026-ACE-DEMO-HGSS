set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_C_COMPILER_WORKS 1)

set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")
set(CMAKE_ASM_COMPILER "arm-none-eabi-as")

set(CMAKE_C_FLAGS "-mcpu=arm946e-s -mtune=arm946e-s -mthumb -march=armv5te -Os -nostartfiles -nodefaultlibs -fno-builtin -ffunction-sections --data-sections -Wl,--gc-sections -fno-unroll-loops -fomit-frame-pointer" )
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -ffreestanding -nostdlib -fno-exceptions -fno-rtti -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-threadsafe-statics")
set(CMAKE_CXX_STANDARD 20)

set(DEFAULT_SYMBOLFILE ${CMAKE_CURRENT_SOURCE_DIR}/tools/symbols/${GAME}.sym)

set(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
