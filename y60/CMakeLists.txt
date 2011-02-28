
cmake_minimum_required(VERSION 2.6.2)

project(Y60)

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
find_package(AcMake)

# Discover dependencies
include(Y60Dependencies.cmake)

# Some magic for tracemonkey
set(Y60_EXTRA_DEFINITIONS)
if(Y60_WITH_TRACEMONKEY)
    set(Y60_EXTRA_DEFINITIONS -DY60_WITH_TRACEMONKEY)
endif(Y60_WITH_TRACEMONKEY)

if(UNIX)
    set(Y60_LAUNCHER_TEMPLATES
        Y60BuildLauncher.sh.in
        Y60InstallLauncher.sh.in
    )
endif(UNIX)

if(WIN32)
    set(Y60_LAUNCHER_TEMPLATES
        Y60InstallLauncher.bat.in
    )
endif(WIN32)

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
        Y60RenderTest.cmake.in
        ${Y60_LAUNCHER_TEMPLATES}
    DEFINITIONS
        ${Y60_EXTRA_DEFINITIONS}
)

# Make sure we can access our includes without staging
include_directories(../ ${CMAKE_CURRENT_BINARY_DIR}/../)
ac_project_add_build_include_dir(${CMAKE_CURRENT_BINARY_DIR}/../)
ac_project_add_build_include_dir(${CMAKE_CURRENT_SOURCE_DIR}/../)

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
if(Y60_SDL_IS_FRAMEWORK)
    add_subdirectory(sdlmain)
endif(Y60_SDL_IS_FRAMEWORK)


# Handle optional gtk support
if(GTKMM_FOUND AND GTKGLEXT_FOUND AND GLADEMM_FOUND)
    option(Y60_WITH_GTK "Build javascript gtk bindings and g60 frontend" ON)
endif(GTKMM_FOUND AND GTKGLEXT_FOUND AND GLADEMM_FOUND)

# Add optional gtk-based components
if(Y60_WITH_GTK)
    add_subdirectory(acgtk)
    add_subdirectory(jsgtk)
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

# Register executables and plugins/components
add_subdirectory(products)
add_subdirectory(components)

# native js (for tests mainly)
add_subdirectory(js)

add_subdirectory(tests)

# Generate standard documentation
ac_add_doxygen(Y60 ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/doc/doxygen Doxyfile.in)
ac_add_xsddoc(Y60 ${CMAKE_CURRENT_SOURCE_DIR}/base/Y60.xsd ${CMAKE_CURRENT_BINARY_DIR}/doc/schema)

# Generate Y60 javascript api specification in xml and html
make_directory(${CMAKE_CURRENT_BINARY_DIR}/doc/jsapi)
get_target_property(Y60_LOCATION y60 LOCATION)
add_custom_target(
    Y60-jsapi
    ${Y60_LOCATION} create_documentation_xml.js
        ${CMAKE_CURRENT_BINARY_DIR}/doc/jsapi
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS y60
    VERBATIM
)
ac_add_documentation_target(Y60-jsapi)
add_custom_target(
    Y60-jsdoc
    ${Y60_LOCATION} create_documentation.js
        ${CMAKE_CURRENT_BINARY_DIR}/doc/jsdoc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS y60
    VERBATIM
)
ac_add_documentation_target(Y60-jsdoc)

# Install XDG MIME definition
if(LINUX)
    install(
        FILES mime.xml
        RENAME y60.xml
        DESTINATION share/mime/packages
    )
endif(LINUX)

ac_end_project(Y60)
