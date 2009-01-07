
cmake_minimum_required(VERSION 2.6)

project(Y60)

# Make our build definitions available
list(INSERT CMAKE_MODULE_PATH 0 ${Y60_SOURCE_DIR})

# XXX: There is no way for adding this in the UI on WIN32.
#      It's goddamn ugly though.
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_INSTALL_PREFIX}/share/cmake-2.6/Modules)

# Include ACMake
include(AcMake)

# Define some things and discover dependencies
include(Y60Definitions)
include(Y60Dependencies)
include(Y60Utilities)

# Make sure we can access our includes without staging
include_directories(../)

# Traverse our subdirectories
add_subdirectory(base)
add_subdirectory(sound)
add_subdirectory(image)
add_subdirectory(video)
add_subdirectory(scene)
add_subdirectory(glutil)
add_subdirectory(gltext)
add_subdirectory(glrender)
add_subdirectory(input)
add_subdirectory(inet)
add_subdirectory(modelling)
add_subdirectory(jsbase)
add_subdirectory(jslib)
add_subdirectory(sdlengine)

if(GTKMM_FOUND AND GTKGLEXT_FOUND AND GLADEMM_FOUND)
    add_subdirectory(acgtk)
    add_subdirectory(jsgtk)
endif(GTKMM_FOUND AND GTKGLEXT_FOUND AND GLADEMM_FOUND)

add_subdirectory(products)
add_subdirectory(components)

# Install script library
install(
    DIRECTORY js
    DESTINATION lib/y60
    FILES_MATCHING
        PATTERN "*.js"
        PATTERN ".svn" EXCLUDE
)

# Install shader library
install(
    DIRECTORY shader
    DESTINATION lib/y60
    FILES_MATCHING
        PATTERN "*.cg"
        PATTERN "*.xml"
        PATTERN "*.png"
        PATTERN "*.rgb"
        PATTERN ".svn" EXCLUDE
)

# Export cmake package unless in an integrated build
if(NOT Y60_INTEGRATED_BUILD)
configure_file(
    ${Y60_SOURCE_DIR}/FindY60.cmake.in
    ${Y60_BINARY_DIR}/FindY60.cmake
    @ONLY
)

install(
    FILES Y60Dependencies.cmake Y60Definitions.cmake Y60Utilities.cmake
    DESTINATION lib/cmake
)

install(
    FILES ${Y60_BINARY_DIR}/FindY60.cmake
    DESTINATION lib/cmake
)

install(
    EXPORT Y60
    DESTINATION lib/cmake
    FILE Y60Export.cmake
)
endif(NOT Y60_INTEGRATED_BUILD)
