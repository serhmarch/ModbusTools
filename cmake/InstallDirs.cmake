if (WIN32)
    # Flat layout — all files next to the executables
    set(MBTOOLS_INSTALL_BIN_DIR    ".")
    set(MBTOOLS_INSTALL_LIB_DIR    ".")
    set(MBTOOLS_INSTALL_SCRIPT_DIR "script/server")
    set(MBTOOLS_INSTALL_HELP_DIR   "help")
else()
    # FHS layout for Linux
    include(GNUInstallDirs)
    set(MBTOOLS_INSTALL_BIN_DIR    "${CMAKE_INSTALL_BINDIR}")
    set(MBTOOLS_INSTALL_LIB_DIR    "${CMAKE_INSTALL_LIBDIR}/mbtools")
    set(MBTOOLS_INSTALL_SCRIPT_DIR "${CMAKE_INSTALL_DATAROOTDIR}/mbtools/script/server")
    set(MBTOOLS_INSTALL_HELP_DIR   "${CMAKE_INSTALL_DATAROOTDIR}/mbtools/help")
endif()
