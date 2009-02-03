
cmake_minimum_required(VERSION 2.6)

project(ASL)

# XXX: There is no way for adding this in the UI on WIN32.
#      It's goddamn ugly though. Also note that we APPEND.
if(NOT ASL_INTEGRATED_BUILD)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_INSTALL_PREFIX}/share/cmake-2.6/Modules)
endif(NOT ASL_INTEGRATED_BUILD)

# Include cmake libraries
include(AcMake)

# XXX: definition handling
add_definitions(-DAC_BUILT_WITH_CMAKE)

# Define dependencies
set(REQ_PACKAGES ZLIB GLIB2 CURL OpenEXR)
set(REQ_PKGCONFIG)

if(WIN32)
    list(APPEND REQ_PACKAGES PTHREADVC DirectX)
endif(WIN32)

if(LINUX)
    list(APPEND REQ_PKGCONFIG ALSA alsa)
endif(LINUX)

# XXX: framework handling
if(OSX)
    find_library(CARBON     Carbon)
    find_library(AUDIO_UNIT AudioUnit)
endif(OSX)

# Define the project
ac_add_project(
    ASL
    REQUIRED_PACKAGES
        ${REQ_PACKAGES}
    REQUIRED_PKGCONFIG
        ${REQ_PKGCONFIG}
)

# Make sure we can access our includes without staging
include_directories(../)

# Traverse our subdirectories
add_subdirectory(base)
add_subdirectory(math)
add_subdirectory(audio)
add_subdirectory(raster)
add_subdirectory(dom)
add_subdirectory(xpath)
add_subdirectory(serial)
add_subdirectory(net)
add_subdirectory(ipc)
add_subdirectory(zip)

# Documentation
ac_add_doxygen(ASL ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/doc/doxygen Doxyfile.in)

ac_export_project(ASL)
