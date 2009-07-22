
cmake_minimum_required(VERSION 2.6.2)

project(ASL)

# Include cmake libraries
include(AcMake)

set(ACMAKE_BUILDINFO_TEMPLATE
    ${CMAKE_CURRENT_SOURCE_DIR}/base/buildinfo.cpp.in)

set(Boost_ADDITIONAL_VERSIONS 1.37 1.38)

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
    mark_as_advanced(CARBON)
    mark_as_advanced(AUDIO_UNIT)
endif(OSX)

ac_add_project(
    ASL
    REQUIRED_PACKAGES
        ${REQ_PACKAGES}
    REQUIRED_PKGCONFIG
        ${REQ_PKGCONFIG}
    DEFINITIONS
        -DAC_BUILT_WITH_CMAKE
    CUSTOM_SCRIPTS
        ASLUtilities.cmake
)

# Make sure we can access our includes without staging
include_directories(../ ${CMAKE_CURRENT_BINARY_DIR}/../)
ac_project_add_build_include_dir(${CMAKE_CURRENT_BINARY_DIR}/../)
ac_project_add_build_include_dir(${CMAKE_CURRENT_SOURCE_DIR}/../)

option(ASL_WITH_THREAD "If the new thread library should be compiled." NO)
option(ASL_WITH_UNUSED "If the aslunused library should be compiled, containing various experimental and obsolete components." YES)

# Traverse our subdirectories
add_subdirectory(base)
if(ASL_WITH_THREAD)
    add_subdirectory(thread)
endif(ASL_WITH_THREAD)
add_subdirectory(math)
add_subdirectory(audio)
add_subdirectory(raster)
add_subdirectory(dom)
add_subdirectory(xpath)
add_subdirectory(serial)
add_subdirectory(net)
add_subdirectory(ipc)
add_subdirectory(zip)
add_subdirectory(utilities)
if(ASL_WITH_UNUSED)
    add_subdirectory(unused)
endif(ASL_WITH_UNUSED)

ac_add_doxygen(ASL ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/doc/doxygen Doxyfile.in)

ac_end_project(ASL)
