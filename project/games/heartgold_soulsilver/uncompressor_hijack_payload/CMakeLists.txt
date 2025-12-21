cmake_minimum_required(VERSION 3.22)
project(uncompressor_hijack_payload)

set(DOT_ARTIST_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/main.c )

add_executable(uncompressor_hijack_payload ${SOURCE} )

target_include_directories(uncompressor_hijack_payload PRIVATE ${COMMON_LIB_DIR} ${DOT_ARTIST_SOURCE})

set(LINKERFILE ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)

set_target_properties(uncompressor_hijack_payload PROPERTIES
    LINK_FLAGS "-T${LINKERFILE} -Wl,--just-symbols=${SYMBOLFILE}"
)

add_custom_command(
    TARGET uncompressor_hijack_payload POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:uncompressor_hijack_payload> ${BIN_DIR}/uncompressor_hijack_payload.bin
    COMMENT "Generating binary"
)

add_custom_command(
    TARGET uncompressor_hijack_payload POST_BUILD
    COMMAND ${CMAKE_OBJDUMP} -d $<TARGET_FILE:uncompressor_hijack_payload> > ${OBJDUMP_DIR}/uncompressor_hijack_payload.dump
    COMMENT "Generating object dump"
)

add_custom_target(build_uncompressor_hijack_payload ALL
    DEPENDS uncompressor_hijack_payload
    DEPENDS ${BIN_DIR}/uncompressor_hijack_payload.bin
    DEPENDS ${OBJDUMP_DIR}/uncompressor_hijack_payload.dump
)
