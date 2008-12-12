
cmake_minimum_required(VERSION 2.6)

project(sdlttf)

include(FindPkgConfig)

pkg_search_module(SDL      REQUIRED sdl)
pkg_search_module(FREETYPE REQUIRED freetype2)

include_directories(.)

link_directories(${SDL_LIBRARY_DIRS} ${FREETYPE_LIBRARY_DIRS})
include_directories(${SDL_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS})

add_library(sdlttf SHARED SDL_ttf.c)

set_target_properties(
    sdlttf
    PROPERTIES
        PUBLIC_HEADER "SDL_ttf.h"
)

target_link_libraries(sdlttf ${SDL_LIBRARIES} ${FREETYPE_LIBRARIES})

install(
    TARGETS sdlttf
    EXPORT  sdlttf
    LIBRARY
         DESTINATION lib
    PUBLIC_HEADER
         DESTINATION include/sdl/ttf2
)

install(
    EXPORT sdlttf
    DESTINATION lib/cmake
)

