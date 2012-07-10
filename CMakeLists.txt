
enable_testing()

cmake_minimum_required(VERSION 2.6.2)

project(PRO60)

list(INSERT CMAKE_PREFIX_PATH 0 ${CMAKE_BINARY_DIR})

find_package(AcMake)
find_package(ASL)

# declare our packaging solution
ac_begin_solution(
    PRO60
    VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 7
    DESCRIPTION  "Y60 Media Engine"
    VENDOR       "ART+COM AG"
    CONTACT      "y60@artcom.de"
    DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/DESCRIPTION.txt"
    LICENSE_FILE     "${CMAKE_SOURCE_DIR}/LICENSE.txt"
    PACKAGE_ICON     "${CMAKE_SOURCE_DIR}/installer/windows/NSIS-Header.bmp"
    INSTALLER_ICON   "${CMAKE_SOURCE_DIR}/installer/windows/shopping_bag.ico"
    UNINSTALLER_ICON "${CMAKE_SOURCE_DIR}/installer/windows/shopping_bag_grey.ico"
)

# define debian packages
if(ACMAKE_DEBIAN)
    ac_debian_define_source(
        pro60
        SECTION graphics
        PRIORITY optional
        MAINTAINER
            "<y60@artcom.de>"
        HOMEPAGE
            "http://y60.artcom.de/"
        PROVIDE_DEBUG_PACKAGE
    )

    ac_debian_add_package(
        acmake
        DESCRIPTION "ART+COM CMake Library"
        ARCHITECTURE all
        LONG_DESCRIPTION
            "AcMake is a set of CMake modules primarily used for building the Y60 Media Engine."
        DEPENDS cmake
        RECOMMENDS cdbs
        COMPONENTS "acmake"
    )
endif(ACMAKE_DEBIAN)

# add our packages
add_subdirectory(netsrc)
add_subdirectory(y60)

ac_end_solution(PRO60)

if(ACMAKE_DEBIAN)
    message("FINALIZING DEBIAN PACKAGES")
    ac_debian_finalize()
endif(ACMAKE_DEBIAN)

