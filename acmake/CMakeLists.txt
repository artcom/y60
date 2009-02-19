
cmake_minimum_required(VERSION 2.6)

project(AcMake)

# installation paths for modules and templates
set(ACMAKE_TOOLS_SUB share/cmake-2.6/Tools)
set(ACMAKE_MODULES_SUB share/cmake-2.6/Modules)
set(ACMAKE_TEMPLATES_SUB share/cmake-2.6/Templates)
set(ACMAKE_INCLUDE_SUB include )

# generate config file describing installation
set(ACMAKE_INSTALLED_TOOLS_DIR ${CMAKE_INSTALL_PREFIX}/${ACMAKE_TOOLS_SUB})
set(ACMAKE_INSTALLED_MODULES_DIR ${CMAKE_INSTALL_PREFIX}/${ACMAKE_MODULES_SUB})
set(ACMAKE_INSTALLED_TEMPLATES_DIR ${CMAKE_INSTALL_PREFIX}/${ACMAKE_TEMPLATES_SUB})
set(ACMAKE_INSTALLED_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/${ACMAKE_INCLUDE_SUB})
configure_file(AcMakeConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/AcMakeConfig.cmake @ONLY)

# install config file
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/AcMakeConfig.cmake
    DESTINATION ${ACMAKE_MODULES_SUB}
)

# install modules
install(
    DIRECTORY modules/
    DESTINATION ${ACMAKE_MODULES_SUB}
    FILES_MATCHING
        PATTERN "*.cmake"
        PATTERN ".svn" EXCLUDE
)

# install templates
install(
    DIRECTORY templates/
    DESTINATION ${ACMAKE_TEMPLATES_SUB}
    FILES_MATCHING
        PATTERN "*.in"
        PATTERN ".svn" EXCLUDE
)

# install tools
set(EXTRA_TOOLS)
if(WIN32)
    list(APPEND EXTRA_TOOLS PATTERN "*.bat")
endif(WIN32)
install(
    DIRECTORY tools/
    DESTINATION ${ACMAKE_TOOLS_SUB}
    FILES_MATCHING
        PATTERN "*.jar"
        ${EXTRA_TOOLS}
	PATTERN ".svn" EXCLUDE
)

#install acmake headers
install(
    DIRECTORY include/
    DESTINATION ${ACMAKE_INCLUDE_SUB}
    FILES_MATCHING
        PATTERN "*.h"
	PATTERN ".svn" EXCLUDE
)
