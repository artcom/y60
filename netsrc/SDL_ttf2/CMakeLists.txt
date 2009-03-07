
cmake_minimum_required(VERSION 2.6.2)

project(ACSDLTTF)

include(AcMake)

include_directories(.)

# stage our header
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/SDL_ttf.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/sdl/ttf2/SDL_ttf.h
    COPYONLY
)

ac_add_project(
    ACSDLTTF
    REQUIRED_PACKAGES SDL Freetype
    DEFINITIONS
        -DAC_BUILT_WITH_CMAKE
)

ac_add_library(
    acsdlttf sdl/ttf2
    SOURCES SDL_ttf.c
    HEADERS SDL_ttf.h
    EXTERNS SDL FREETYPE
)

_ac_add_include_path(acsdlttf ${CMAKE_CURRENT_BINARY_DIR}/include YES)

ac_end_project(ACSDLTTF)
