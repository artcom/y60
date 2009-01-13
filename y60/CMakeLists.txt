
cmake_minimum_required(VERSION 2.6)

project(Y60)

# Make our build definitions available
list(INSERT CMAKE_MODULE_PATH 0 ${Y60_SOURCE_DIR})

# XXX: There is no way for adding this in the UI on WIN32.
#      It's goddamn ugly though. Also note that we APPEND.
if(NOT Y60_INTEGRATED_BUILD)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_INSTALL_PREFIX}/share/cmake-2.6/Modules)
endif(NOT Y60_INTEGRATED_BUILD)

# Include ACMake
include(AcMake)

# Define some things and discover dependencies
include(Y60Definitions)
include(Y60Dependencies)
include(Y60Utilities)

# XXX: Spidermonkey definitions should be dependency-propagated.
add_definitions(${SPIDERMONKEY_DEFINITIONS})

# Make sure we can access our includes without staging
include_directories(../)
include_directories(../netsrc/paintlib/src)

# XXX: Make headers from our netsrc dependencies available.
#      This is needed because we do not handle paths for
#      cmake-exported libraries correctly at the moment.
#      It would work if the references in our libs would
#      be EXTERNS, but since they are DEPENDS, we do not add 
#      include paths anywhere and need this hack for now.
if(NOT Y60_INTEGRATED_BUILD)
    include_directories(${CMAKE_INSTALL_PREFIX}/include)
endif(NOT Y60_INTEGRATED_BUILD)

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

# Generate standard documentation
ac_add_doxygen(Y60 ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/doc/doxygen Doxyfile.in)
ac_add_xsddoc(Y60 ${CMAKE_CURRENT_SOURCE_DIR}/base/Y60.xsd ${CMAKE_CURRENT_BINARY_DIR}/doc/schema)

# Generate Y60 javascript api reference
make_directory(${CMAKE_CURRENT_BINARY_DIR}/doc/jsapi)
add_custom_target(
    Y60-jsapi
    ${CMAKE_CURRENT_BINARY_DIR}/products/y60/y60
        -I "${CMAKE_CURRENT_SOURCE_DIR}/js;"
	${CMAKE_CURRENT_SOURCE_DIR}/js/create_documentation_xml.js
	${CMAKE_CURRENT_BINARY_DIR}/doc/jsapi
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}    
    DEPENDS y60
    VERBATIM
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
    DESTINATION share/cmake-2.6/Modules
)

install(
    FILES ${Y60_BINARY_DIR}/FindY60.cmake
    DESTINATION share/cmake-2.6/Modules
)

install(
    EXPORT Y60
    DESTINATION share/cmake-2.6/Modules
    FILE Y60Export.cmake
)
endif(NOT Y60_INTEGRATED_BUILD)
