# doxygen Configuration

cmake_minimum_required(VERSION 3.13) # 2.2 - case insensitive syntax
                                     # 3.13 included policy CMP0077

find_program(MBTOOLS_DOXYGEN_EXECUTABLE
    NAMES doxygen doxygen.exe
    PATHS
        "$ENV{ProgramFiles}\\Doxygen\\bin"
        "$ENV{ProgramFiles\(x86\)}\\Doxygen\\bin"
    DOC "Doxygen documentation generator"
)

if(MBTOOLS_DOXYGEN_EXECUTABLE)
    message(STATUS "MBTOOLS-DOC: Found Doxygen: ${MBTOOLS_DOXYGEN_EXECUTABLE}")
    project("mbtools-doc" VERSION ${MBTOOLS_VERSION})

    find_program(MBTOOLS_DOT_EXECUTABLE
        NAMES dot dot.exe
        PATHS
            "$ENV{ProgramFiles}\\Graphviz\\bin"
            "$ENV{ProgramFiles\(x86\)}\\Graphviz\\bin"
        DOC "Graphviz dot executable"
    )

    if(MBTOOLS_DOT_EXECUTABLE)
        get_filename_component(MBTOOLS_DOC_DOT_PATH "${MBTOOLS_DOT_EXECUTABLE}" DIRECTORY)
        set(MBTOOLS_DOC_HAVE_DOT YES)
        message(STATUS "MBTOOLS-DOC: Found Graphviz dot: ${MBTOOLS_DOT_EXECUTABLE}")
    else()
        set(MBTOOLS_DOC_DOT_PATH "")
        set(MBTOOLS_DOC_HAVE_DOT NO)
        message(WARNING "MBTOOLS-DOC: Graphviz dot not found. Class/collaboration graphs will be disabled in generated docs.")
    endif()

    set(MBTOOLS_DOC_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}")
    set(MBTOOLS_DOC_DIR "${CMAKE_BINARY_DIR}/doc")

    # Ensure documentation output directories exist before running Doxygen
    file(MAKE_DIRECTORY "${MBTOOLS_DOC_DIR}")
    file(MAKE_DIRECTORY "${MBTOOLS_DOC_DIR}/output")

    if (MBTOOLS_CLIENT_ENABLED)
        set(MBTOOLS_DOC_CLIENT_FILE_NAME "DoxyfileClient")
        set(MBTOOLS_DOC_CLIENT_FILE "${MBTOOLS_DOC_DIR}/${MBTOOLS_DOC_CLIENT_FILE_NAME}")

        file(MAKE_DIRECTORY "${MBTOOLS_DOC_DIR}/output/client")

        message(STATUS "MBTOOLS-DOC: Create '${MBTOOLS_DOC_CLIENT_FILE}'")
        configure_file(${CMAKE_CURRENT_LIST_DIR}/${MBTOOLS_DOC_CLIENT_FILE_NAME}.in
                    ${MBTOOLS_DOC_CLIENT_FILE})

        add_custom_target(doxygen-client ALL
                WORKING_DIRECTORY "${MBTOOLS_DOC_DIR}"
                COMMAND "${MBTOOLS_DOXYGEN_EXECUTABLE}" "${MBTOOLS_DOC_CLIENT_FILE}"
                COMMENT "Build doxygen documentation for client"
                )

        add_dependencies(doxygen-client ${MBTOOLS_CORE_LIB_NAME}
                                        ${MBTOOLS_CLIENT_APP_NAME})

    endif()

    if (MBTOOLS_SERVER_ENABLED)
        set(MBTOOLS_DOC_SERVER_FILE_NAME "DoxyfileServer")
        set(MBTOOLS_DOC_SERVER_FILE "${MBTOOLS_DOC_DIR}/${MBTOOLS_DOC_SERVER_FILE_NAME}")

        file(MAKE_DIRECTORY "${MBTOOLS_DOC_DIR}/output/server")

        message(STATUS "MBTOOLS-DOC: Create '${MBTOOLS_DOC_SERVER_FILE}'")
        configure_file(${CMAKE_CURRENT_LIST_DIR}/${MBTOOLS_DOC_SERVER_FILE_NAME}.in
                    ${MBTOOLS_DOC_SERVER_FILE})

        add_custom_target(doxygen-server ALL
                WORKING_DIRECTORY "${MBTOOLS_DOC_DIR}"
                COMMAND "${MBTOOLS_DOXYGEN_EXECUTABLE}" "${MBTOOLS_DOC_SERVER_FILE}"
                COMMENT "Build doxygen documentation for server"
                )

        add_dependencies(doxygen-server ${MBTOOLS_CORE_LIB_NAME}
                                        ${MBTOOLS_SERVER_APP_NAME})
    
    endif()

else()
    message(WARNING "MBTOOLS-DOC: Doxygen not found. Documentation will not be generated.")
    message(STATUS "MBTOOLS-DOC: To install Doxygen, visit: https://www.doxygen.nl/download.html")
endif()

