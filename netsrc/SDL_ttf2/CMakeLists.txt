
cmake_minimum_required(VERSION 2.6)

project(SDLTTF)

set(CMAKE_MODULE_PATH ${SDLTTF_SOURCE_DIR})

include(SDLTTFDefinitions)
include(SDLTTFDependencies)

add_definitions(-DAC_BUILT_WITH_CMAKE)

include_directories(.)

link_directories(${SDL_LIBRARY_DIRS} ${FREETYPE_LIBRARY_DIRS})
include_directories(${SDL_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS})

add_library(SDL_ttf2 SHARED SDL_ttf.c)

set_target_properties(
    SDL_ttf2
    PROPERTIES
        PUBLIC_HEADER "SDL_ttf.h"
)

target_link_libraries(SDL_ttf2 ${SDL_LIBRARIES} ${FREETYPE_LIBRARIES})

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/SDL_ttf.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/sdl/ttf2/SDL_ttf.h
    COPYONLY)


install(
    TARGETS SDL_ttf2
    EXPORT SDLTTF
    LIBRARY
         DESTINATION lib
    PUBLIC_HEADER
         DESTINATION include/sdl/ttf2
)


configure_file(
    ${SDLTTF_SOURCE_DIR}/FindSDLTTF.cmake.in
    ${SDLTTF_BINARY_DIR}/FindSDLTTF.cmake
    @ONLY
)

install(
    FILES ${SDLTTF_BINARY_DIR}/FindSDLTTF.cmake
    DESTINATION lib/cmake
)


install(
    FILES SDLTTFDependencies.cmake SDLTTFDefinitions.cmake
    DESTINATION lib/cmake
)

install(
    EXPORT SDLTTF
    DESTINATION lib/cmake
    FILE SDLTTFExport.cmake
)


