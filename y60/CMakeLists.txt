
cmake_minimum_required(VERSION 2.6.2)

project(Y60)

list(INSERT CMAKE_MODULE_PATH 0 ${Y60_SOURCE_DIR})

option(Y60_WITH_TRACEMONKEY "use latest experimental js interpreter" OFF)

set(ACMAKE_BUILDINFO_TEMPLATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../asl/base/buildinfo.cpp.in)

if (Y60_WITH_TRACEMONKEY)
    set(JSDEPENDS "")
    set(JSEXTERN MOZJS)
else(Y60_WITH_TRACEMONKEY)
    set(JSDEPENDS spidermonkey)
    set(JSEXTERN "")
endif(Y60_WITH_TRACEMONKEY)

# Include ACMake
include(AcMake)

# Define some things and discover dependencies
include(Y60Dependencies)
include(Y60Utilities)

set(Y60_EXTRA_DEFINITIONS)
if(Y60_WITH_TRACEMONKEY)
    set(Y60_EXTRA_DEFINITIONS -DY60_WITH_TRACEMONKEY)
endif(Y60_WITH_TRACEMONKEY)

ac_add_project(Y60
    REQUIRED_PACKAGES
        ${REQ_PACKAGES}
    OPTIONAL_PACKAGES
        ${OPT_PACKAGES}   
    REQUIRED_PKGCONFIG
        ${REQ_PKGCONFIG}
    OPTIONAL_PKGCONFIG
        ${OPT_PKGCONFIG}
    CUSTOM_SCRIPTS
        Y60Utilities.cmake
    CUSTOM_TEMPLATES
        Y60BuildLauncher.sh.in
        Y60InstallLauncher.sh.in
    DEFINITIONS
        -DAC_BUILT_WITH_CMAKE
        ${Y60_EXTRA_DEFINITIONS}
)

ac_add_install_type(
    modelling
    DISPLAY_NAME "Modelling"
)

# Make sure we can access our includes without staging
include_directories(../ ${CMAKE_CURRENT_BINARY_DIR}/../)

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
    option(Y60_WITH_GTK "Build javascript gtk bindings and g60 frontend" ON)
endif(GTKMM_FOUND AND GTKGLEXT_FOUND AND GLADEMM_FOUND)

if(Y60_WITH_GTK)
    add_subdirectory(acgtk)
    add_subdirectory(jsgtk)

    # XXX: these must come after some actual
    #      use of the components due to a CPack bug
    ac_add_installer_component(
        G60_runtime
        DISPLAY_NAME "G60 Runtime"
        DESCRIPTION "Y60 GTK"
    )
    ac_add_installer_component(
        G60_development
        DISPLAY_NAME "G60 Development"
        DESCRIPTION  "Y60 GTK"
    )
endif(Y60_WITH_GTK)

# Install script library
install(
    DIRECTORY js
    DESTINATION lib/y60
    COMPONENT Y60_runtime
    FILES_MATCHING
        PATTERN "*.js"
        PATTERN ".svn" EXCLUDE
)

# Install shader library
install(
    DIRECTORY shader
    DESTINATION lib/y60
    COMPONENT Y60_runtime
    FILES_MATCHING
        PATTERN "*.cg"
        PATTERN "*.xml"
        PATTERN "*.png"
        PATTERN "*.rgb"
        PATTERN ".svn" EXCLUDE
)

add_subdirectory(products)
add_subdirectory(components)

# Generate standard documentation
ac_add_doxygen(Y60 ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/doc/doxygen Doxyfile.in)
ac_add_xsddoc(Y60 ${CMAKE_CURRENT_SOURCE_DIR}/base/Y60.xsd ${CMAKE_CURRENT_BINARY_DIR}/doc/schema)

# Generate Y60 javascript api reference (XXX: broken)
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

ac_end_project(Y60)
