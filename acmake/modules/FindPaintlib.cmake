
include(${CMAKE_SOURCE_DIR}/netsrc/paintlib/PaintlibDefinitions.cmake)
include(${CMAKE_SOURCE_DIR}/netsrc/paintlib/PaintlibDependencies.cmake)

set(PAINTLIB_LIBRARY_DIRS "${CMAKE_BINARY_DIR}/netsrc/paintlib/src")
set(PAINTLIB_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/netsrc/paintlib/include")
set(PAINTLIB_LIBRARIES paintlib)
