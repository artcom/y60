
cmake_minimum_required(VERSION 2.6)

project(ACSDLTTF)

set(CMAKE_MODULE_PATH ${ACSDLTTF_SOURCE_DIR})

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
    ${ACSDLTTF_SOURCE_DIR}/FindACSDLTTF.cmake.in
    ${ACSDLTTF_BINARY_DIR}/FindACSDLTTF.cmake
    @ONLY
)

if(ACSDLTTF_INTEGRATED_BUILD)
    configure_file(SDL_ttf.h ${CMAKE_CURRENT_BINARY_DIR}/include/sdl/ttf2/SDL_ttf.h)
endif(ACSDLTTF_INTEGRATED_BUILD)

if(NOT ACSDLTTF_INTEGRATED_BUILD)

install(
    FILES ${ACSDLTTF_BINARY_DIR}/FindACSDLTTF.cmake
    DESTINATION share/cmake-2.6/Modules
)


install(
    FILES ACSDLTTFDependencies.cmake ACSDLTTFDefinitions.cmake
    DESTINATION share/cmake-2.6/Modules
)

install(
    EXPORT ACSDLTTF
    DESTINATION share/cmake-2.6/Modules
    FILE ACSDLTTFExport.cmake
)

endif(NOT ACSDLTTF_INTEGRATED_BUILD)

