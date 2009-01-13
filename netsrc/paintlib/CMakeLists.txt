
cmake_minimum_required(VERSION 2.6)

project(Paintlib)

include(FindPkgConfig)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${Paintlib_SOURCE_DIR}
		${PROJECT_SOURCE_DIR}/../../acmake/modules)

if(WIN32)
    list( INSERT CMAKE_PREFIX_PATH 0 "$ENV{PRO60_DEPS}")
endif(WIN32)

include(PaintlibDefinitions)
include(PaintlibDependencies)

if (WIN32)
    add_definitions(
        -D_CRT_SECURE_NO_WARNINGS
	    -D_CRT_NONSTDC_NO_WARNINGS
        -D_SCL_SECURE_NO_WARNINGS
    )
endif (WIN32)

add_definitions( -DAC_BUILT_WITH_CMAKE )

add_subdirectory(src)

if(NOT PAINTLIB_INTEGRATED_BUILD)
configure_file(
    ${Paintlib_SOURCE_DIR}/FindPaintlib.cmake.in
    ${Paintlib_BINARY_DIR}/FindPaintlib.cmake
    @ONLY
)

install(
    FILES PaintlibDependencies.cmake PaintlibDefinitions.cmake
    DESTINATION share/cmake-2.6/Modules
)

install(
    FILES ${Paintlib_BINARY_DIR}/FindPaintlib.cmake
    DESTINATION share/cmake-2.6/Modules
)

install(
    EXPORT paintlib
    DESTINATION share/cmake-2.6/Modules
    FILE PaintlibExport.cmake
)
endif(NOT PAINTLIB_INTEGRATED_BUILD)

