cmake_minimum_required(VERSION 3.22)
project(GDQ_DEMO_HGSS)

set(COMMON_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
file(GLOB COMMON_LIB_SRC_FILES
    ${COMMON_LIB_DIR}/*.c
    ${COMMON_LIB_DIR}/*/*.c
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/hooks)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/payloads)