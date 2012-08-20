
cmake_minimum_required(VERSION 2.6.2)

project(TEXTUREATLAS)

include(AcMake)

include_directories(.)

# stage our header
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/TexturePacker.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/texture-atlas/TexturePacker.h
    COPYONLY
)

ac_add_project(
    TEXTUREATLAS
)

ac_add_library(
    textureatlas texture-atlas
    SOURCES TexturePacker.cpp 
    HEADERS  TexturePacker.h textureatlas_settings.h
    VERSION ${VERSION_STRING}
    SOVERSION ${VERSION_MAJOR}
    RUNTIME_INSTALL_COMPONENT ${RUNTIME_COMPONENT}
    DEVELOPMENT_INSTALL_COMPONENT ${DEVELOPMENT_COMPONENT}
)

_ac_add_include_path(textureatlas ${CMAKE_CURRENT_BINARY_DIR}/include YES)

ac_end_project(TEXTUREATLAS DONT_INSTALL)
