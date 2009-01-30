
cmake_minimum_required(VERSION 2.6)

project(ACSDLTTF)

list(INSERT CMAKE_MODULE_PATH 0 ${ACSDLTTF_SOURCE_DIR})

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

set(LIBRARY_DIRS
    ${SDL_LIBRARY_DIR} ${FREETYPE_LIBRARY_DIRS}
)

if(SDL_LIBRARY_DIRS_D)
    list(APPEND LIBRARY_DIRS ${SDL_LIBRARY_DIRS_D})
endif(SDL_LIBRARY_DIRS_D)

if(FREETYPE_LIBRARY_DIRS_D)
    list(APPEND LIBRARY_DIRS ${FREETYPE_LIBRARY_DIRS_D})
endif(FREETYPE_LIBRARY_DIRS_D)

link_directories(${LIBRARY_DIRS})
include_directories(${SDL_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIRS})

add_library(acsdlttf SHARED SDL_ttf.c)

set_target_properties(
    acsdlttf
    PROPERTIES
        PUBLIC_HEADER "SDL_ttf.h"
        INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
        INSTALL_RPATH_USE_LINK_PATH YES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
)

# XXX: need to expose all libraries as transitive link deps
set(OPTIMIZED_LIBRARIES ${FREETYPE_LIBRARIES} ${SDL_LIBRARY})

set(DEBUG_LIBRARIES)
if(FREETYPE_LIBRARIES_D)
    list(APPEND DEBUG_LIBRARIES ${FREETYPE_LIBRARIES_D})
endif(FREETYPE_LIBRARIES_D)
if(SDL_LIBRARY_D)
    list(APPEND DEBUG_LIBRARIES ${SDL_LIBRARY_D})
endif(SDL_LIBRARY_D)

if(DEBUG_LIBRARIES)
    foreach(SUBLIB ${OPTIMIZED_LIBRARIES})
        target_link_libraries(acsdlttf optimized ${SUBLIB})
    endforeach(SUBLIB)
    foreach(SUBLIB ${DEBUG_LIBRARIES})
        target_link_libraries(acsdlttf debug ${SUBLIB})
    endforeach(SUBLIB)
else(DEBUG_LIBRARIES)
    target_link_libraries(acsdlttf ${OPTIMIZED_LIBRARIES})
endif(DEBUG_LIBRARIES)

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

