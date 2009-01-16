
cmake_minimum_required(VERSION 2.6)

project(ACSDLTTF)

set(CMAKE_MODULE_PATH ${ACSDLTTF_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/../../acmake/modules)

if(WIN32)
    list( INSERT CMAKE_PREFIX_PATH 0 "$ENV{PRO60_DEPS}")
    # Make CMake build all libs in a common directory. Since we build
    # DLLs the tests need to find their libraries even while they are not
    # installed in their final locations (during testrun that is).
    set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
endif(WIN32)

include(ACSDLTTFDefinitions)
include(ACSDLTTFDependencies)

add_definitions(-DAC_BUILT_WITH_CMAKE)

include_directories(.)

link_directories(${SDL_LIBRARY_DIR} ${FREETYPE_LIBRARY_DIRS})
include_directories(${SDL_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIRS})

add_library(acsdlttf SHARED SDL_ttf.c)

set_target_properties(
    acsdlttf
    PROPERTIES
        PUBLIC_HEADER "SDL_ttf.h"
)

target_link_libraries(acsdlttf ${SDL_LIBRARY} ${FREETYPE_LIBRARIES})

install(
    TARGETS acsdlttf
        DESTINATION lib # XXX: This is a hack! TARGETS does not need or have a DESTINATION.
                           #	But for some reason it does not work without it! [DS]
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

