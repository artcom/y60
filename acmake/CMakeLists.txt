
cmake_minimum_required(VERSION 2.6)

project(AcMake)

# installation paths for our files
if(WIN32)
    set(ACMAKE_MODULES_SUB cmake)
elseif(APPLE)
    set(ACMAKE_MODULES_SUB ".")
elseif(UNIX)
    set(ACMAKE_MODULES_SUB lib/acmake/cmake)
endif(WIN32)
set(ACMAKE_TOOLS_SUB lib/acmake/tools)
set(ACMAKE_TEMPLATES_SUB lib/acmake/templates)
set(ACMAKE_INCLUDE_SUB include)

# determine absolute build time locations of our files
set(ACMAKE_INSTALLED_TOOLS_DIR ${CMAKE_INSTALL_PREFIX}/${ACMAKE_TOOLS_SUB})
set(ACMAKE_INSTALLED_MODULES_DIR ${CMAKE_INSTALL_PREFIX}/${ACMAKE_MODULES_SUB})
set(ACMAKE_INSTALLED_TEMPLATES_DIR ${CMAKE_INSTALL_PREFIX}/${ACMAKE_TEMPLATES_SUB})
set(ACMAKE_INSTALLED_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/${ACMAKE_INCLUDE_SUB})

# generate the config script
configure_file(AcMakeConfig.cmake.in ${CMAKE_BINARY_DIR}/${ACMAKE_MODULES_SUB}/AcMakeConfig.cmake @ONLY)

# install config file
install(
    FILES ${CMAKE_BINARY_DIR}/${ACMAKE_MODULES_SUB}/AcMakeConfig.cmake
    COMPONENT AcMake
    DESTINATION ${ACMAKE_MODULES_SUB}
)

# install modules
install(
    DIRECTORY modules/
    DESTINATION ${ACMAKE_MODULES_SUB}
    COMPONENT AcMake
    FILES_MATCHING
        PATTERN "*.cmake"
        PATTERN ".svn" EXCLUDE
)

# install templates
install(
    DIRECTORY templates/
    DESTINATION ${ACMAKE_TEMPLATES_SUB}
    COMPONENT AcMake
    FILES_MATCHING
        PATTERN "*.in"
        PATTERN ".svn" EXCLUDE
)

# install tools
set(EXTRA_TOOLS)
if(WIN32)
    list(APPEND EXTRA_TOOLS PATTERN "*.bat")
    list(APPEND EXTRA_TOOLS PATTERN "*.wsf")
    list(APPEND EXTRA_TOOLS PATTERN "*.js")
endif(WIN32)
install(
    DIRECTORY tools/
    DESTINATION ${ACMAKE_TOOLS_SUB}
    COMPONENT AcMake
    FILES_MATCHING
        PATTERN "*.jar"
        ${EXTRA_TOOLS}
	PATTERN ".svn" EXCLUDE
)

#install acmake headers
#install(
#    DIRECTORY include/
#    DESTINATION ${ACMAKE_INCLUDE_SUB}
#    COMPONENT AcMake
#    FILES_MATCHING
#        PATTERN "*.h"
#	PATTERN ".svn" EXCLUDE
#)
