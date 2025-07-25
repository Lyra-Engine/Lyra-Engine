include(GNUInstallDirs)

set(PROJECT_NAME "lyra")
set(PROJECT_PREFIX "")
set(PROJECT_COMMON_LIB "lyra::engine")

# specify the binary directory
set(LIBRARY_BIN_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
if(NOT EXISTS ${LIBRARY_BIN_DIRECTORY})
    # create the directory
    file(MAKE_DIRECTORY ${LIBRARY_BIN_DIRECTORY})
    message(STATUS "Bin Directory: ${LIBRARY_BIN_DIRECTORY}")
endif()

# define a macro for shared library
macro(lyra_shared NAME)
    set(TARGET_NAME    "${PROJECT_NAME}-${NAME}")
    set(NAMESPACE_NAME "${PROJECT_NAME}::${NAME}")

    # add library target
    add_library(${TARGET_NAME} SHARED)

    # change the dll path (to allow easier linking)
    set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_BIN_DIRECTORY}/
        LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_BIN_DIRECTORY}/
        ARCHIVE_OUTPUT_DIRECTORY ${LIBRARY_BIN_DIRECTORY}/)

    # re-export target with namespace
    add_library(${NAMESPACE_NAME} ALIAS ${TARGET_NAME})

    # IDE target folders
    set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "")
    set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Plugins")

    # install shared library
    install(
        TARGETS ${TARGET_NAME}
        EXPORT ${TARGET_NAME}-targets
        DESTINATION ${CMAKE_INSTALL_LIBDIR})

endmacro()

# define a macro for plugin registration
macro(lyra_plugin NAME)
    set(TARGET_NAME    "${PROJECT_NAME}-${NAME}")
    set(NAMESPACE_NAME "${PROJECT_NAME}::${NAME}")

    # add module target
    add_library(${TARGET_NAME} MODULE)

    # change the dll path (to allow easier linking)
    set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_BIN_DIRECTORY}/
        LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_BIN_DIRECTORY}/
        ARCHIVE_OUTPUT_DIRECTORY ${LIBRARY_BIN_DIRECTORY}/)

    # re-export target with namespace
    add_library(${NAMESPACE_NAME} ALIAS ${TARGET_NAME})

    # IDE target folders
    set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "")
    set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Plugins")
    target_link_libraries(${TARGET_NAME} PUBLIC ${PROJECT_COMMON_LIB})

endmacro()

# define a macro for sample registration
macro(lyra_sample NAME)
    set(TARGET_NAME    "${PROJECT_NAME}-${NAME}")
    set(NAMESPACE_NAME "${PROJECT_NAME}::${NAME}")

    # add executable target
    add_executable(${TARGET_NAME})

    # change the dll path (to allow easier linking)
    set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_BIN_DIRECTORY}/
        LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_BIN_DIRECTORY}/
        ARCHIVE_OUTPUT_DIRECTORY ${LIBRARY_BIN_DIRECTORY}/)

    # re-export target with namespace
    add_executable(${NAMESPACE_NAME} ALIAS ${TARGET_NAME})

    set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "")
    set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Samples")
    target_link_libraries(${TARGET_NAME} PUBLIC ${PROJECT_COMMON_LIB})
endmacro()
