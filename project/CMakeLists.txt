cmake_minimum_required(VERSION 3.22)
project(project_builder)

enable_language(ASM)

set(OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_custom_target(make_output_dir ALL)

add_custom_command(TARGET make_output_dir PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUT_DIR}
    COMMENT "Creating output directory at ${OUT_DIR}"
)

add_subdirectory(${SOURCE_DIR})

add_custom_target(project_builder ALL DEPENDS make_output_dir)
