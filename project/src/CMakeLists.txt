cmake_minimum_required(VERSION 3.22)
project(GDQ_DEMO_HGSS)

set(COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB COMMON_LIB_SRC_FILES
    ${COMMON_DIR}/libs/*.c
    ${COMMON_DIR}/libs/*/*.c
)

set(PREPROCESSOR_DIR ${OUT_DIR}/hooks/preprocessor)

add_subdirectory(hooks)
add_subdirectory(payloads)
add_dependencies(all_payloads all_hooks)