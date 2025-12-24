cmake_minimum_required(VERSION 3.22)
project(GDQ_DEMO_HGSS)

set(COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB COMMON_LIB_SRC_FILES
    ${COMMON_DIR}/libs/*.c
    ${COMMON_DIR}/libs/*/*.c
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/hooks)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/payloads)