cmake_minimum_required(VERSION 3.22)
project(assembly_projects)

set(BIN_DIR ${OUT_DIR}/hooks/bins)
set(OBJDUMP_DIR ${OUT_DIR}/hooks/dumps)

add_custom_target(make_hooks_dirs ALL)
add_custom_command(TARGET make_hooks_dirs PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OBJDUMP_DIR}
    COMMENT "Creating directories for hooks subproject"
)

file(GLOB SUBDIRS LIST_DIRECTORIES true ${CMAKE_CURRENT_SOURCE_DIR}/*)

foreach(SUBDIR IN LISTS SUBDIRS)
    if(IS_DIRECTORY ${SUBDIR})
        get_filename_component(OUTPUT_NAME ${SUBDIR} NAME)
        get_filename_component(PROJECT_NAME ${SUBDIR}_hook NAME)
        set(SOURCE_FILE ${SUBDIR}/hook.asm)

        add_executable(${PROJECT_NAME} ${SOURCE_FILE})
        set(LINKER_FILE ${SUBDIR}/linker.ld)
        target_link_options(${PROJECT_NAME} PRIVATE -T ${LINKER_FILE})

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