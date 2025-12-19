cmake_minimum_required(VERSION 3.22)
project(boot_hijack_payload)

set(DOT_ARTIST_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/main.c )

add_executable(boot_hijack_payload ${SOURCE} )

target_include_directories(boot_hijack_payload PRIVATE ${COMMON_LIB_DIR} ${DOT_ARTIST_SOURCE})

set(LINKERFILE ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)

set_target_properties(boot_hijack_payload PROPERTIES
    LINK_FLAGS "-T${LINKERFILE} -Wl,--just-symbols=${SYMBOLFILE}"
)

add_custom_command(
    TARGET boot_hijack_payload POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:boot_hijack_payload> ${BIN_DIR}/boot_hijack_payload.bin
    COMMENT "Generating binary"
)

add_custom_command(
    TARGET boot_hijack_payload POST_BUILD
    COMMAND ${CMAKE_OBJDUMP} -d $<TARGET_FILE:boot_hijack_payload> > ${OBJDUMP_DIR}/boot_hijack_payload.dump
    COMMENT "Generating object dump"
)

add_custom_target(build_boot_hijack_payload ALL
    DEPENDS boot_hijack_payload
    DEPENDS ${BIN_DIR}/boot_hijack_payload.bin
    DEPENDS ${OBJDUMP_DIR}/boot_hijack_payload.dump
)
