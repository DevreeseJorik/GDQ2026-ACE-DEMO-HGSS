cmake_minimum_required(VERSION 3.22)
project(hgss_payloads)

set(BIN_DIR ${OUT_DIR}/payloads/bins)
set(OBJDUMP_DIR ${OUT_DIR}/payloads/dumps)

add_custom_target(make_payloads_dirs ALL)
add_custom_command(TARGET make_payloads_dirs PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OBJDUMP_DIR}
    COMMENT "Creating directories for payloads subproject"
)

set(SYMBOLFILE ${DEFAULT_SYMBOLFILE})

add_custom_target(all_payloads ALL)
add_dependencies(all_payloads make_payloads_dirs)

file(GLOB SUBDIRS LIST_DIRECTORIES true ${CMAKE_CURRENT_SOURCE_DIR}/*)

foreach(SUB_DIR IN LISTS SUBDIRS)
    if(IS_DIRECTORY ${SUB_DIR})
        get_filename_component(OUTPUT_NAME ${SUB_DIR} NAME)
        get_filename_component(PROJECT_NAME ${SUB_DIR}_payload NAME)

        file(GLOB LOCAL_SRC_FILES
            LIST_DIRECTORIES false
            ${SUB_DIR}/*.c
            ${SUB_DIR}/*.cpp
            ${SUB_DIR}/src/*.c
            ${SUB_DIR}/src/*.cpp
        )

        add_executable(${PROJECT_NAME} ${COMMON_SRC_FILES} ${COMMON_LIB_SRC_FILES} ${LOCAL_SRC_FILES})
        target_include_directories(${PROJECT_NAME} PRIVATE ${COMMON_DIR} ${SUB_DIR}/src ${PREPROCESSOR_DIR})

        set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${OUTPUT_NAME})

        set(LINKER_FILE ${SUB_DIR}/linker.ld)
        target_link_options(${PROJECT_NAME} PRIVATE -T ${LINKER_FILE} "-Wl,--just-symbols=${SYMBOLFILE}")

        add_dependencies(${PROJECT_NAME} all_payloads)

        add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -O binary
                    $<TARGET_FILE:${PROJECT_NAME}>
                    ${BIN_DIR}/${OUTPUT_NAME}.bin
            COMMENT "Generating binary for ${PROJECT_NAME}"
        )

        add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_OBJDUMP} -d
                    $<TARGET_FILE:${PROJECT_NAME}>
                    > ${OBJDUMP_DIR}/${OUTPUT_NAME}.dump
            COMMENT "Generating object dump for ${PROJECT_NAME}"
        )

        add_custom_target(build_${PROJECT_NAME} ALL
            DEPENDS ${PROJECT_NAME}
            DEPENDS ${BIN_DIR}/${OUTPUT_NAME}.bin
            DEPENDS ${OBJDUMP_DIR}/${OUTPUT_NAME}.dump
        )

    endif()
endforeach()
