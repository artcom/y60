
cmake_minimum_required(VERSION 2.6)

project(SDLTTF)

set(CMAKE_MODULE_PATH ${SDLTTF_SOURCE_DIR})

include(SDLTTFDefinitions)
include(SDLTTFDependencies)


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


