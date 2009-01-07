
cmake_minimum_required(VERSION 2.6)

project(ASL)

# Make our build definitions available
list(INSERT CMAKE_MODULE_PATH 0 ${ASL_SOURCE_DIR})

# XXX: There is no way for adding this in the UI on WIN32.
#      It's goddamn ugly though.
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_INSTALL_PREFIX}/share/cmake-2.6/Modules)

# XXX: Make headers from our netsrc dependencies available.
#      This is needed because we do not handle paths for
#      cmake-exported libraries correctly at the moment.
#      It would work if the references in our libs would
#      be EXTERNS, but since they are DEPENDS, we do not add 
#      include paths anywhere and need this hack for now.
if(NOT ASL_INTEGRATED_BUILD)
    include_directories(${CMAKE_INSTALL_PREFIX}/include)
endif(NOT ASL_INTEGRATED_BUILD)

# Include ACMake
include(AcMake)

# Define some things and discover dependencies
include(ASLDefinitions)
include(ASLDependencies)

# Make sure we can access our includes without staging
include_directories(../)

# Traverse our subdirectories
add_subdirectory(base)
add_subdirectory(math)
add_subdirectory(audio)
add_subdirectory(raster)
add_subdirectory(dom)
add_subdirectory(xpath)
add_subdirectory(serial)
add_subdirectory(net)
add_subdirectory(ipc)
add_subdirectory(zip)

# Generate exports unless in an integrated build
if(NOT ASL_INTEGRATED_BUILD)
configure_file(
    ${ASL_SOURCE_DIR}/FindASL.cmake.in
    ${ASL_BINARY_DIR}/FindASL.cmake
    @ONLY
)

install(
    FILES ASLDependencies.cmake ASLDefinitions.cmake
    DESTINATION share/cmake-2.6/Modules
)

install(
    FILES ${ASL_BINARY_DIR}/FindASL.cmake
    DESTINATION share/cmake-2.6/Modules
)

install(
    EXPORT ASL
    DESTINATION share/cmake-2.6/Modules
    FILE ASLExport.cmake
)
endif(NOT ASL_INTEGRATED_BUILD)
