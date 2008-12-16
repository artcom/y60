
cmake_minimum_required(VERSION 2.6)

project(Paintlib)

set(CMAKE_MODULE_PATH ${Paintlib_SOURCE_DIR})

include(PaintlibDefinitions)
include(PaintlibDependencies)

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
