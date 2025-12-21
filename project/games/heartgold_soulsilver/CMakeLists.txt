cmake_minimum_required(VERSION 3.22)
project(heartgold_soulsilver)

set(COMMON_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
file(GLOB COMMON_LIB_SRC_FILES ${COMMON_LIB_DIR}/*/*.c)

set(SUB_PROJECTS
    initial_payload
    boot_hijack_payload
    boot_hijack_hook
    cardi_hijack_payload
    cardi_hijack_hook
    uncompressor_hijack_payload
    uncompressor_hijack_hook
    sprite_editing
    sprite_narc_hook
    sprite_inject_hook
    palette_inject_hook
)

foreach(SUB_PROJECT IN LISTS SUB_PROJECTS)
    set(SUB_DIR ${GAME_OUT_DIR}/${SUB_PROJECT})
    set(BIN_DIR ${SUB_DIR}/bin)
    set(OBJDUMP_DIR ${SUB_DIR}/dump)

    add_custom_target(make_${GAME}_${SUB_PROJECT}_dirs ALL)

    add_custom_command(TARGET make_${GAME}_${SUB_PROJECT}_dirs PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SUB_DIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_DIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${OBJDUMP_DIR}
        COMMENT "Creating directories for subproject: ${SUB_PROJECT}"
    )

    add_subdirectory(${SUB_PROJECT})
endforeach()
