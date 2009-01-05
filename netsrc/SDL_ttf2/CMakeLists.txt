
cmake_minimum_required(VERSION 2.6)

project(SDLTTF)

set(CMAKE_MODULE_PATH ${SDLTTF_SOURCE_DIR})

include(ACSDLTTFDefinitions)
include(ACSDLTTFDependencies)

add_definitions(-DAC_BUILT_WITH_CMAKE)

include_directories(.)

link_directories(${SDL_LIBRARY_DIRS} ${FREETYPE_LIBRARY_DIRS})
include_directories(${SDL_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS})

add_library(acsdlttf SHARED SDL_ttf.c)

set_target_properties(
    acsdlttf
    PROPERTIES
        PUBLIC_HEADER "SDL_ttf.h"
)

target_link_libraries(acsdlttf ${SDL_LIBRARIES} ${FREETYPE_LIBRARIES})

install(
    TARGETS acsdlttf
    EXPORT ACSDLTTF
    LIBRARY
         DESTINATION lib
    PUBLIC_HEADER
         DESTINATION include/sdl/ttf2 # XXX: inappropriate location. consider package name.
)


configure_file(
    ${SDLTTF_SOURCE_DIR}/FindACSDLTTF.cmake.in
    ${SDLTTF_BINARY_DIR}/FindACSDLTTF.cmake
    @ONLY
)

install(
    FILES ${SDLTTF_BINARY_DIR}/FindACSDLTTF.cmake
    DESTINATION lib/cmake
)


install(
    FILES ACSDLTTFDependencies.cmake ACSDLTTFDefinitions.cmake
    DESTINATION lib/cmake
)

install(
    EXPORT ACSDLTTF
    DESTINATION lib/cmake
    FILE ACSDLTTFExport.cmake
)

