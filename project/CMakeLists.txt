cmake_minimum_required(VERSION 3.22)
project(project_builder)

if(NOT DEFINED GAME)
  message(FATAL_ERROR "GAME variable must be set (e.g. -DGAME=POKEMON_SS_IPGE_01) before configuring the project.")
endif()

set(TEMP_SYMBOL_FILE "${CMAKE_CURRENT_SOURCE_DIR}/tools/symbols/${GAME}.sym")
if(NOT EXISTS "${TEMP_SYMBOL_FILE}")
  file(GLOB AVAILABLE_SYMBOLS "${CMAKE_CURRENT_SOURCE_DIR}/tools/symbols/*.sym")
  string(REPLACE ";" "\n  " AVAILABLE_LIST "${AVAILABLE_SYMBOLS}")
  message(FATAL_ERROR "Symbol file '${TEMP_SYMBOL_FILE}' does not exist for GAME='${GAME}'.\nAvailable symbol files:\n  ${AVAILABLE_LIST}")
endif()

enable_language(ASM C CXX)

set(OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_custom_target(make_output_dir ALL)

add_custom_command(TARGET make_output_dir PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUT_DIR}
    COMMENT "Creating output directory at ${OUT_DIR}"
)

add_subdirectory(${SOURCE_DIR})

add_custom_target(project_builder ALL DEPENDS make_output_dir)
