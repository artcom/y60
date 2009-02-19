
cmake_minimum_required(VERSION 2.6.2)

project(ASL)

# Include cmake libraries
include(AcMake)

set(ACMAKE_BUILDINFO_TEMPLATE
    ${CMAKE_CURRENT_SOURCE_DIR}/base/buildinfo.cpp.in)

set(REQ_PACKAGES ZLIB GLIB2 CURL OpenEXR Boost)
if(WIN32)
    list(APPEND REQ_PACKAGES PTHREADVC DirectX WINSOCK Winmm Psapi)
endif(WIN32)

set(REQ_PKGCONFIG)
if(LINUX)
    list(APPEND REQ_PKGCONFIG ALSA alsa)
endif(LINUX)

# XXX: framework handling
if(OSX)
    find_library(CARBON     Carbon)
    find_library(AUDIO_UNIT AudioUnit)
    mark_as_advanced( CARBON )
    mark_as_advanced( AUDIO_UNIT )
endif(OSX)

ac_add_project(
    ASL
    REQUIRED_PACKAGES
        ${REQ_PACKAGES}
    REQUIRED_PKGCONFIG
        ${REQ_PKGCONFIG}
    DEFINITIONS
        -DAC_BUILT_WITH_CMAKE
)

# Make sure we can access our includes without staging
include_directories(../)

# XXX: Make headers from our netsrc dependencies available.
#      This is needed because we do not handle paths for
#      cmake-exported libraries correctly at the moment.
#      It would work if the references in our libs would
#      be EXTERNS, but since they are DEPENDS, we do not add 
#      include paths anywhere and need this hack for now.
if(NOT ASL_INTEGRATED_BUILD)
    include_directories(${CMAKE_INSTALL_PREFIX}/include)
endif(NOT ASL_INTEGRATED_BUILD)

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

ac_add_doxygen(ASL ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/doc/doxygen Doxyfile.in)

ac_end_project(ASL)
