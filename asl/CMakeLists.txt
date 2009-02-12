
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

set(REQ_PACKAGES ZLIB GLIB2 CURL OpenEXR)
if(WIN32)
    list(APPEND REQ_PACKAGES PTHREADVC DirectX)
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

ac_export_project(ASL)
