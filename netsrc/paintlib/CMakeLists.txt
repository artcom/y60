
cmake_minimum_required(VERSION 2.6)

project(Paintlib)

set(CMAKE_MODULE_PATH ${Paintlib_SOURCE_DIR} ${CMAKE_MODULE_PATH} $ENV{ACMAKE}/modules)

include(FindPkgConfig)

include(PaintlibDefinitions)
include(PaintlibDependencies)

if (WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS
            -D_SCL_SECURE_NO_WARNINGS)
endif (WIN32)

add_definitions( -DAC_BUILT_WITH_CMAKE )

add_subdirectory(src)

configure_file(
    ${Paintlib_SOURCE_DIR}/FindPaintlib.cmake.in
    ${Paintlib_BINARY_DIR}/FindPaintlib.cmake
    @ONLY
)

install(
    FILES PaintlibDependencies.cmake PaintlibDefinitions.cmake
    DESTINATION lib/cmake
)

install(
    FILES ${Paintlib_BINARY_DIR}/FindPaintlib.cmake
    DESTINATION lib/cmake
)

install(
    EXPORT paintlib
    DESTINATION lib/cmake
    FILE PaintlibExport.cmake
)
