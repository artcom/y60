
cmake_minimum_required(VERSION 2.6)

project(AcMake)

# installation paths for modules and templates
set(ACMAKE_MODULES_SUB share/cmake-2.6/Modules)
set(ACMAKE_TEMPLATES_SUB share/cmake-2.6/Templates)

set(ACMAKE_INSTALLED_MODULES_DIR ${CMAKE_INSTALL_PREFIX}/${ACMAKE_MODULES_SUB})
set(ACMAKE_INSTALLED_TEMPLATES_DIR ${CMAKE_INSTALL_PREFIX}/${ACMAKE_TEMPLATES_SUB})

configure_file(AcMakeConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/AcMakeConfig.cmake @ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/AcMakeConfig.cmake
    DESTINATION ${ACMAKE_MODULES_SUB}
)

install(
    DIRECTORY modules/
    DESTINATION ${ACMAKE_MODULES_SUB}
    FILES_MATCHING
        PATTERN "*.cmake"
        PATTERN ".svn" EXCLUDE
)

install(
    DIRECTORY templates/
    DESTINATION ${ACMAKE_TEMPLATES_SUB}
    FILES_MATCHING
        PATTERN "*.in"
        PATTERN ".svn" EXCLUDE
)

