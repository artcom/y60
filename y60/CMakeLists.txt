
cmake_minimum_required(VERSION 2.6)

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

pro60_deps_install()

ac_add_project(Y60
    REQUIRED_PACKAGES
        ${REQ_PACKAGES}
    OPTIONAL_PACKAGES
        ${OPT_PACKAGES}   
    REQUIRED_PKGCONFIG
        ${REQ_PKGCONFIG}
    CUSTOM_SCRIPTS
        Y60Utilities
    DEFINITIONS
        -DAC_BUILT_WITH_CMAKE
        ${Y60_EXTRA_DEFINITIONS}
)

# Make sure we can access our includes without staging
include_directories(../)

# XXX: Make headers from our netsrc dependencies available.
#      This is needed because we do not handle paths for
#      cmake-exported libraries correctly at the moment.
#      It would work if the references in our libs would
#      be EXTERNS, but since they are DEPENDS, we do not add 
#      include paths anywhere and need this hack for now.
# XXX: this should be completely obsolete by now
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
    option(Y60_WITH_GTK "Build javascript gtk bindings and g60 frontend" ON)
endif(GTKMM_FOUND AND GTKGLEXT_FOUND AND GLADEMM_FOUND)

if(Y60_WITH_GTK)
    add_subdirectory(acgtk)
    add_subdirectory(jsgtk)
endif(Y60_WITH_GTK)

if(OSX)
    option(Y60_MAC_OS_X_BUNDLE
            "Build y60 as a Mac OS X bundle instead of a unix executable"
            OFF)
endif(OSX)

if(Y60_MAC_OS_X_BUNDLE)
    file(GLOB_RECURSE Y60_SCRIPT_FILES js/*.js)
    ac_create_resource_set( Y60_RESOURCES
                   "${CMAKE_CURRENT_SOURCE_DIR}"
                   "Resources"
                   "${Y60_SCRIPT_FILES}")
    list(APPEND SHADER_EXTENSIONS "*.cg" "*.xml" "*.png" "*.rgb" )
    foreach( EXT ${SHADER_EXTENSIONS})
        file(GLOB_RECURSE Y60_SHADER_FILES "shader/${EXT}")
        ac_add_resources( Y60_RESOURCES "${Y60_SHADER_FILES}")
    endforeach( EXT SHADER_EXTENSIONS)
else(Y60_MAC_OS_X_BUNDLE)
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
endif(Y60_MAC_OS_X_BUNDLE)

add_subdirectory(products)
add_subdirectory(components)

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

ac_end_project(Y60)
