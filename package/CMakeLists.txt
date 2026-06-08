message(STATUS "MBTOOLS-PACK: CPack packaging enabled")

set(CPACK_PACKAGE_NAME                  "${MBTOOLS_NAME}")
set(CPACK_PACKAGE_VERSION               "${MBTOOLS_VERSION}")
set(CPACK_PACKAGE_VENDOR                "${MBTOOLS_VENDOR}")
set(CPACK_PACKAGE_CONTACT               "${MBTOOLS_CONTACT}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY   "${MBTOOLS_DESCRIPTION_SUMMARY}")
set(CPACK_PACKAGE_DESCRIPTION           "${MBTOOLS_DESCRIPTION}")
set(CPACK_PACKAGE_HOMEPAGE_URL          "${MBTOOLS_HOMEPAGE_URL}")
set(CPACK_RESOURCE_FILE_LICENSE         "${MBTOOLS_ROOT_DIR}/${MBTOOLS_LICENSE}")
set(CPACK_RESOURCE_FILE_README          "${MBTOOLS_ROOT_DIR}/${MBTOOLS_README}")

# Monolithic IFW package (no CPack components).
set(CPACK_MONOLITHIC_INSTALL ON)
set(CPACK_GENERATOR "IFW")

set(CPACK_IFW_PACKAGE_TITLE "ModbusTools Setup")
set(CPACK_IFW_PACKAGE_PUBLISHER "${MBTOOLS_VENDOR}")
set(CPACK_IFW_PRODUCT_URL "${MBTOOLS_HOMEPAGE_URL}")
set(CPACK_IFW_PACKAGE_WIZARD_STYLE "Modern")
set(CPACK_IFW_PACKAGE_MAINTENANCE_TOOL_NAME "mbtools-maintenancetool")
set(CPACK_IFW_PACKAGE_OFFLINE_ONLY ON)
set(CPACK_IFW_PACKAGE_CONTROL_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/ifw/installscript.qs")

if (WIN32)
	set(CPACK_PACKAGE_INSTALL_DIRECTORY "mbtools")
else()
	# Keep Linux package layout under /usr according to install-layout skill.
	set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
endif()

if (NOT CPACK_IFW_ROOT)
	if (DEFINED ENV{CPACK_IFW_ROOT} AND NOT "$ENV{CPACK_IFW_ROOT}" STREQUAL "")
		set(CPACK_IFW_ROOT "$ENV{CPACK_IFW_ROOT}")
	elseif (WIN32)
		set(CPACK_IFW_ROOT "C:/Qt/Tools/QtInstallerFramework/4.11")
	else()
		set(CPACK_IFW_ROOT "/opt/Qt/Tools/QtInstallerFramework/4.11")
	endif()
endif()

include(CPackIFW)

include(CPack)