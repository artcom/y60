
enable_testing()

cmake_minimum_required(VERSION 2.6.2)

project(PRO60)

# make build-integrated acmake available
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/acmake/modules)

# make build-integrated packages available
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/asl)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_BINARY_DIR}/asl)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/y60)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_BINARY_DIR}/y60)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_BINARY_DIR}/netsrc/SDL_ttf2)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_BINARY_DIR}/netsrc/paintlib)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_BINARY_DIR}/netsrc/oscpack)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_BINARY_DIR}/netsrc/fakeatl)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/netsrc/fakeatl) # XXX: [IA] this should not be needed
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_BINARY_DIR}/netsrc/spidermonkey) # XXX: tracemonkey?

# tell acmake that it is being used in integrated mode
set(ACMAKE_INTEGRATED_BUILD YES)

# set the global default buildinfo template
set(ACMAKE_BUILDINFO_TEMPLATE
    ${CMAKE_CURRENT_SOURCE_DIR}/asl/base/buildinfo.cpp.in)



# add and load acmake
add_subdirectory(acmake)
include(AcMake)

# declare our packaging solution
ac_begin_solution(
    PRO60
    VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 2
    DESCRIPTION  "PRO60 Environment"
    VENDOR       "ART+COM Technologies GmbH"
    CONTACT      "info@y60.artcom.de"
    DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/DESCRIPTION.txt"
    LICENSE_FILE     "${CMAKE_SOURCE_DIR}/LICENSE.txt"
    PACKAGE_ICON     "${CMAKE_SOURCE_DIR}/installer/windows/NSIS-Header.bmp"
    INSTALLER_ICON   "${CMAKE_SOURCE_DIR}/installer/windows/shopping_bag.ico"
    UNINSTALLER_ICON "${CMAKE_SOURCE_DIR}/installer/windows/shopping_bag_grey.ico"
)

# define the installer component for AcMake
if(ACMAKE_BUILD_PACKAGES)
cpack_add_component(
    AcMake
    DISPLAY_NAME "AcMake"
    DESCRIPTION "ART+COM CMake Library"
    INSTALL_TYPES development
)
endif(ACMAKE_BUILD_PACKAGES)

# add our packages
add_subdirectory(netsrc)
add_subdirectory(asl)
add_subdirectory(y60)

# integrate testmodels and tutorials if present
if(EXISTS ${CMAKE_SOURCE_DIR}/../testmodels)
  add_subdirectory(../testmodels ${CMAKE_CURRENT_BINARY_DIR}/testmodels)
endif(EXISTS ${CMAKE_SOURCE_DIR}/../testmodels)
if(EXISTS ${CMAKE_SOURCE_DIR}/../tutorials)
  add_subdirectory(../tutorials ${CMAKE_CURRENT_BINARY_DIR}/tutorials)
endif(EXISTS ${CMAKE_SOURCE_DIR}/../tutorials)

ac_end_solution(PRO60)
