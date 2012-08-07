
enable_testing()

cmake_minimum_required(VERSION 2.6.2)

project(Y60)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 7)
set(VERSION_STRING ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

list(INSERT CMAKE_PREFIX_PATH 0 ${CMAKE_BINARY_DIR})


find_package(AcMake)
find_package(ASL)

# declare our packaging solution
ac_begin_solution(
    Y60
    VERSION_MAJOR ${VERSION_MAJOR} VERSION_MINOR ${VERSION_MINOR} VERSION_PATCH ${VERSION_PATCH}
    DESCRIPTION  "Y60 Media Engine"
    VENDOR       "ART+COM AG"
    CONTACT      "info@artcom.de"
    DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/DESCRIPTION.txt"
    LICENSE_FILE     "${CMAKE_SOURCE_DIR}/LICENSE.txt"
    PACKAGE_ICON     "${CMAKE_SOURCE_DIR}/installer/windows/NSIS-Header.bmp"
    INSTALLER_ICON   "${CMAKE_SOURCE_DIR}/installer/windows/shopping_bag.ico"
    UNINSTALLER_ICON "${CMAKE_SOURCE_DIR}/installer/windows/shopping_bag_grey.ico"
    CREATE_SHORTCUTS TRUE
)

# declare cpack components
set(RUNTIME_COMPONENT Y60_runtime)
set(DEVELOPMENT_COMPONENT Y60_development)

cpack_add_component( ${RUNTIME_COMPONENT}
            DISPLAY_NAME "Runtime"
            DESCRIPTION "runtime libraries" REQUIRED)
cpack_add_component( ${DEVELOPMENT_COMPONENT}
            DISPLAY_NAME "Development"
            DESCRIPTION "header files and import libraries")


# define debian packages
if(ACMAKE_DEBIAN)
    ac_debian_define_source(
        Y60
        SECTION graphics
        PRIORITY optional
        MAINTAINER
            "<info@artcom.de>"
        HOMEPAGE
            "https://github.com/artcom/y60"
        PROVIDE_DEBUG_PACKAGE
    )
endif(ACMAKE_DEBIAN)

# add our packages
add_subdirectory(netsrc)
add_subdirectory(y60)

ac_end_solution(Y60)

if(ACMAKE_DEBIAN)
    message("FINALIZING DEBIAN PACKAGES")
    ac_debian_finalize()
endif(ACMAKE_DEBIAN)

