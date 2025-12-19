cmake_minimum_required(VERSION 3.22)
project(initial_payload)

set(DOT_ARTIST_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/main.c )

add_executable(initial_payload ${SOURCE} )

target_include_directories(initial_payload PRIVATE ${COMMON_LIB_DIR} ${DOT_ARTIST_SOURCE})

set(LINKERFILE ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)

set_target_properties(initial_payload PROPERTIES
    LINK_FLAGS "-T${LINKERFILE} -Wl,--just-symbols=${SYMBOLFILE}"
)

add_custom_command(
    TARGET initial_payload POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:initial_payload> ${BIN_DIR}/initial_payload.bin
    COMMENT "Generating binary"
)

add_custom_command(
    TARGET initial_payload POST_BUILD
    COMMAND ${CMAKE_OBJDUMP} -d $<TARGET_FILE:initial_payload> > ${OBJDUMP_DIR}/initial_payload.dump
    COMMENT "Generating object dump"
)

add_custom_target(build_initial_payload ALL
    DEPENDS initial_payload
    DEPENDS ${BIN_DIR}/initial_payload.bin
    DEPENDS ${OBJDUMP_DIR}/initial_payload.dump
)
