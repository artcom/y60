# Some utilities for building Y60-related things

find_package(AcMake)

function(y60_begin_application NAME)
    parse_arguments(
        APP
        "DISPLAY_NAME;DESCRIPTION;DEPENDS;INSTALL_TYPES"
        ""
        ${ARGN}
    )

    # Check for nesting
    if(Y60_CURRENT_APPLICATION)
        message(FATAL_ERROR "Y60 application ${NAME} nested in ${Y60_CURRENT_APPLICATION}")
    endif(Y60_CURRENT_APPLICATION)

    set(Y60_CURRENT_APPLICATION ${NAME} PARENT_SCOPE)

    set_global(${NAME}_NAME ${NAME})

    set_global(${NAME}_IS_APPLICATION YES)

    set_global(${NAME}_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
    set_global(${NAME}_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

    set_global(${NAME}_BUILD_PATH   "${CMAKE_CURRENT_BINARY_DIR};${CMAKE_CURRENT_SOURCE_DIR}")
    set_global(${NAME}_INSTALL_PATH ".")

    # XXX: passthrough for late installer registration
    set_global(${NAME}_DISPLAY_NAME  ${APP_DISPLAY_NAME})
    set_global(${NAME}_DESCRIPTION   ${APP_DESCRIPTION})
    set_global(${NAME}_INSTALL_TYPES ${APP_INSTALL_TYPES})
    set_global(${NAME}_DEPENDS       ${APP_DEPENDS})
endfunction(y60_begin_application)

function(y60_add_asset FILE)
    set(APPLICATION ${Y60_CURRENT_APPLICATION})

    install(
        FILES "${FILE}"
        DESTINATION "lib/${APPLICATION}"
        COMPONENT ${APPLICATION}
    )
endfunction(y60_add_asset)

function(y60_add_assets DIRECTORY)
    parse_arguments(
        THIS_ASSETS
        "PATTERNS"
        "ADD_TO_PATH"
        ${ARGN}
    )

    set(APPLICATION ${Y60_CURRENT_APPLICATION})

    if(NOT THIS_ASSETS_PATTERNS)
        set(THIS_ASSETS_PATTERNS PATTERN "*")
    endif(NOT THIS_ASSETS_PATTERNS)

    get_filename_component(DIRECTORY_NAME "${DIRECTORY}" NAME)

    install(
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}/"
        DESTINATION "lib/${APPLICATION}/${DIRECTORY_NAME}"
        COMPONENT ${APPLICATION}
        FILES_MATCHING
           ${THIS_ASSETS_PATTERNS}
           PATTERN ".svn" EXCLUDE
    )

    if(THIS_ASSETS_ADD_TO_PATH)
        append_global(${APPLICATION}_BUILD_PATH   "${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}")
        append_global(${APPLICATION}_INSTALL_PATH "${DIRECTORY_NAME}")
    endif(THIS_ASSETS_ADD_TO_PATH)
endfunction(y60_add_assets)

macro(y60_add_component COMPONENT_NAME)
    ac_add_plugin(
        ${COMPONENT_NAME} y60/components
        ${ARGN}
        # XXX: install component
    )
    # XXX: semi-dirty path hack
    if(UNIX AND Y60_CURRENT_APPLICATION)
        append_global(${Y60_CURRENT_APPLICATION}_BUILD_PATH ${CMAKE_CURRENT_BINARY_DIR})
    endif(UNIX AND Y60_CURRENT_APPLICATION)
endmacro(y60_add_component)

macro(y60_add_launcher NAME)
    parse_arguments(
        THIS_LAUNCHER
        "COMMAND_NAME;DESCRIPTION;CATEGORIES;ENGINE;MAIN_SCRIPT;BUILD_WORKING_DIR;INSTALL_WORKING_DIR;MIME_TYPES"
        ""
        ${ARGN}
    )

    set(APPLICATION ${Y60_CURRENT_APPLICATION})

    get_global(${APPLICATION}_BUILD_PATH   THIS_APPLICATION_BUILD_PATH)
    get_global(${APPLICATION}_INSTALL_PATH THIS_APPLICATION_INSTALL_PATH)
    get_global(${APPLICATION}_BINARY_DIR   THIS_APPLICATION_BINARY_DIR)
    get_global(${APPLICATION}_SOURCE_DIR   THIS_APPLICATION_SOURCE_DIR)

    set(THIS_LAUNCHER_NAME "${NAME}")

    if(NOT THIS_LAUNCHER_ENGINE)
        set(THIS_LAUNCHER_ENGINE y60)
    endif(NOT THIS_LAUNCHER_ENGINE)

    if(NOT THIS_LAUNCHER_COMMAND_NAME)
        string(TOLOWER "${THIS_LAUNCHER_NAME}" THIS_LAUNCHER_COMMAND_NAME)
    endif(NOT THIS_LAUNCHER_COMMAND_NAME)

    # handle being called by an importer
    get_global(Y60_IS_INTEGRATED Y60_IS_INTEGRATED)
    if(Y60_IS_INTEGRATED)
            get_global(Y60_SOURCE_DIR Y60_SOURCE_DIR)
            set(Y60_TEMPLATE_DIR ${Y60_SOURCE_DIR})
    else(Y60_IS_INTEGRATED)
            set(Y60_TEMPLATE_DIR ${Y60_INSTALL_PREFIX}/share/cmake-2.6/Templates)
    endif(Y60_IS_INTEGRATED)

    # choose working dir for running from build tree
    if(THIS_LAUNCHER_BUILD_WORKING_DIR STREQUAL SOURCE)
        # BINARY lets us run in the source directory of the build
        set(THIS_LAUNCHER_BUILD_WORKING_DIR ${THIS_APPLICATION_SOURCE_DIR})
    elseif(THIS_LAUNCHER_BUILD_WORKING_DIR STREQUAL BINARY)
        # BINARY lets us run in the binary directory of the build
        set(THIS_LAUNCHER_BUILD_WORKING_DIR ${THIS_APPLICATION_BINARY_DIR})
    elseif(NOT THIS_LAUNCHER_BUILD_WORKING_DIR)
        # the prefered default is to run from ${PWD}
        set(THIS_LAUNCHER_BUILD_WORKING_DIR "")
    endif(THIS_LAUNCHER_BUILD_WORKING_DIR STREQUAL SOURCE)

    # choose working dir for running from install
    if(NOT THIS_LAUNCHER_INSTALL_WORKING_DIR)
        # the prefered default is to run from ${PWD}
        set(THIS_LAUNCHER_INSTALL_WORKING_DIR "")
    endif(NOT THIS_LAUNCHER_INSTALL_WORKING_DIR)

    get_property(THIS_LAUNCHER_BUILD_ENGINE TARGET ${THIS_LAUNCHER_ENGINE} PROPERTY LOCATION_RELEASE)
    set(THIS_LAUNCHER_INSTALL_ENGINE ${THIS_LAUNCHER_ENGINE})

    if(UNIX)
#         # generate launcher shell script for build tree runs
#         configure_file(
#             ${Y60_TEMPLATE_DIR}/Y60BuildLauncher.sh.in
#             ${CMAKE_CURRENT_BINARY_DIR}/${THIS_LAUNCHER_COMMAND_NAME}
#             @ONLY
#         )
#         # generate launcher shell script for installed tree runs
#         configure_file(
#             ${Y60_TEMPLATE_DIR}/Y60InstallLauncher.sh.in
#             ${CMAKE_CURRENT_BINARY_DIR}/${ACMAKE_BINARY_SUBDIR}/${THIS_LAUNCHER_COMMAND_NAME}
#             @ONLY
#         )
#         install(
#             FILES ${CMAKE_CURRENT_BINARY_DIR}/${ACMAKE_BINARY_SUBDIR}/${THIS_LAUNCHER_COMMAND_NAME}
#             COMPONENT ${APPLICATION}
#             DESTINATION bin/
#             PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE
#                         OWNER_READ    GROUP_READ    WORLD_READ
#                         OWNER_WRITE
#         )
    endif(UNIX)

    if(LINUX)
#         # generate xdg desktop entry
#         set(THIS_LAUNCHER_DESKTOP_FILE ${CMAKE_CURRENT_BINARY_DIR}/${THIS_LAUNCHER_COMMAND_NAME}.desktop)

#         file(WRITE  ${THIS_LAUNCHER_DESKTOP_FILE} "[Desktop Entry]\n")
#         file(APPEND ${THIS_LAUNCHER_DESKTOP_FILE} "Type=Application\n")
#         file(APPEND ${THIS_LAUNCHER_DESKTOP_FILE} "Name=${THIS_LAUNCHER_NAME}\n")
#         file(APPEND ${THIS_LAUNCHER_DESKTOP_FILE} "TryExec=${THIS_LAUNCHER_COMMAND_NAME}\n")
#         file(APPEND ${THIS_LAUNCHER_DESKTOP_FILE} "Exec=${THIS_LAUNCHER_COMMAND_NAME} %U\n")
#         file(APPEND ${THIS_LAUNCHER_DESKTOP_FILE} "Categories=${THIS_LAUNCHER_CATEGORIES}\n")
#         if(THIS_LAUNCHER_MIME_TYPES)
#             file(APPEND ${THIS_LAUNCHER_DESKTOP_FILE} "MimeType=${THIS_LAUNCHER_MIME_TYPES}\n")
#         endif(THIS_LAUNCHER_MIME_TYPES)

#         install(
#             FILES ${THIS_LAUNCHER_DESKTOP_FILE}
#             COMPONENT ${APPLICATION}
#             DESTINATION share/applications
#         )
    endif(LINUX)

    if(WIN32)
#         # generate launcher batch script for installed tree runs
#         configure_file(
#             ${Y60_TEMPLATE_DIR}/Y60InstallLauncher.bat.in
#             ${CMAKE_CURRENT_BINARY_DIR}/${ACMAKE_BINARY_SUBDIR}/${NAME}.bat
#             @ONLY
#         )
#         install(
#             FILES ${CMAKE_CURRENT_BINARY_DIR}/${ACMAKE_BINARY_SUBDIR}/${NAME}.bat
#             COMPONENT ${APPLICATION}
#             DESTINATION bin/
#             PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE
#                         OWNER_READ    GROUP_READ    WORLD_READ
#                         OWNER_WRITE
#         )

#         ac_add_installer_shortcut(${NAME} "" "" "" "bin\\\\\\\\${NAME}.bat" "")
    endif(WIN32)
endmacro(y60_add_launcher)

function(y60_end_application NAME)
    if(NOT Y60_CURRENT_APPLICATION)
        message(FATAL_ERROR "Trying to end Y60 application ${NAME} before it started")
    endif(NOT Y60_CURRENT_APPLICATION)

    get_global(${NAME}_BUILD_PATH BUILD_PATH)
    get_global(${NAME}_INSTALL_PATH INSTALL_PATH)

    get_global(${NAME}_DISPLAY_NAME  APP_DISPLAY_NAME)
    get_global(${NAME}_DESCRIPTION   APP_DESCRIPTION)
    get_global(${NAME}_INSTALL_TYPES APP_INSTALL_TYPES)
    get_global(${NAME}_DEPENDS       APP_DEPENDS)

    if(ACMAKE_CURRENT_PROJECT)
          ac_add_installer_component(
              ${NAME}
              DISPLAY_NAME "${APP_DISPLAY_NAME}"
              DESCRIPTION  "${APP_DESCRIPTION}"
              INSTALL_TYPES ${APP_INSTALL_TYPES}
              DEPENDS       ${APP_DEPENDS}
          )
    endif(ACMAKE_CURRENT_PROJECT)

    set(Y60_CURRENT_APPLICATION)
endfunction(y60_end_application)


macro(collect_path INTO)
    # collect paths for all plugins
    collect_plugin_path(_PLUGIN_PATH)

    # add some hard-coded defaults
    set(_PATH
        ${Y60_SOURCE_DIR}/js     # javascript library
        ${Y60_SOURCE_DIR}/shader # shader library
        ${_PLUGIN_PATH}          # plugin locations
    )

    # set the output variable
    set(${INTO} ${_PATH})
endmacro(collect_path)

macro(collect_plugin_path INTO) 
    # get list of all plugins in the Y60 project
    get_global(Y60_PLUGINS PLUGINS)

    # compile list of paths based on locations of plugins
    set(PLUGIN_DIRS)
    foreach(PLUGIN ${PLUGINS})
        get_target_property(PLUGIN_FILE ${PLUGIN} LOCATION_${CMAKE_BUILD_TYPE}) 

        get_filename_component(PLUGIN_PATH ${PLUGIN_FILE} PATH)

        list(APPEND PLUGIN_DIRS ${PLUGIN_PATH})
    endforeach(PLUGIN)

    if(PLUGIN_DIRS)
        list(REMOVE_DUPLICATES PLUGIN_DIRS)
    endif(PLUGIN_DIRS)

    # set output variable
    set(${INTO} ${PLUGIN_DIRS})
endmacro(collect_plugin_path)

macro(y60_add_jstest NAME PREFIX)
    # find y60 executable
    get_target_property(Y60_EXECUTABLE y60 LOCATION_${CMAKE_BUILD_TYPE})
    
    # collect basic Y60_PATH
    collect_path(_BASE_PATH)

    # extend the path with test-specifics
    set(_PATH ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${_BASE_PATH})

    # register the test
    add_test(${PREFIX}_${NAME}
        ${Y60_EXECUTABLE} test${NAME}.tst.js ${ARGN} shaderlibrary.xml -I "${_PATH}"
    )
endmacro(y60_add_jstest)


macro(write_scenetest NAME CMAKEFILE DIRNAME IS_CGTEST TOLERANCE THRESHOLD)
  get_target_property(Y60_EXECUTABLE y60 LOCATION_${CMAKE_BUILD_TYPE}) 
  get_target_property(COMPAREIMAGE_EXECUTABLE y60-compare-image LOCATION_${CMAKE_BUILD_TYPE})
  get_target_property(GENMOVIE_EXECUTABLE y60-gen-movie LOCATION_${CMAKE_BUILD_TYPE})
  get_target_property(GENCOMPRESSEDTEX_EXECUTABLE y60-gen-compressed-tex LOCATION_${CMAKE_BUILD_TYPE})

  collect_path(PLUGIN_DIRS) 
  
  if(IS_CGTEST)
    file(WRITE ${CMAKEFILE} "# testscript for cg scenetest ${NAME}\n")
  else(IS_CGTEST)
    file(WRITE ${CMAKEFILE} "# testscript for no cg scenetest ${NAME}\n")
  endif(IS_CGTEST)
  file(APPEND ${CMAKEFILE} "#   will be called with cmake -P ${CMAKEFILE}\n\n")
  if(IS_CGTEST)
    file(APPEND ${CMAKEFILE} "message(\"*********************** CG *********************\")\n")  
  else(IS_CGTEST)
    file(APPEND ${CMAKEFILE} "message(\"*********************** NO CG *********************\")\n")  
  endif(IS_CGTEST)
    
  if(EXISTS ${TESTMODELDIR}/${NAME}.pre.cmake)
    file(APPEND ${CMAKEFILE} "include(\"${TESTMODELDIR}/${NAME}.pre.cmake\")\n")
    file(APPEND ${CMAKEFILE} "${NAME}_pre(\"${CMAKE_BINARY_DIR}\" \"${TESTMODELDIR}\" \"${GENMOVIE_EXECUTABLE}\" \"${GENCOMPRESSEDTEX_EXECUTABLE}\") \n\n")
  endif(EXISTS ${TESTMODELDIR}/${NAME}.pre.cmake)

  file(APPEND ${CMAKEFILE} "message(\"Executing: ${Y60_EXECUTABLE} -I \"${PLUGIN_DIRS}\" ${SCRIPT} ${SCENE} ${SHADERLIB} rehearsal outputimage=${CMAKE_BINARY_DIR}/${DIRNAME}/TestImages/${NAME} outputsuffix=${IMGSUFFIX}\")\n")

  file(APPEND ${CMAKEFILE} "#generate image\n")
  file(APPEND ${CMAKEFILE} "execute_process(\n")
  if( NOT ${DIRNAME} STREQUAL "tutorials" )
    file(APPEND ${CMAKEFILE} "  COMMAND \"${Y60_EXECUTABLE}\" -I \"${PLUGIN_DIRS}\" \"${SCRIPT}\" \"${SCENE}\" \"${SHADERLIB}\" \"rehearsal outputimage=${CMAKE_BINARY_DIR}/${DIRNAME}/TestImages/${NAME}\" \"outputsuffix=${IMGSUFFIX}\"\n")
  else( NOT ${DIRNAME} STREQUAL "tutorials" )
    file(APPEND ${CMAKEFILE} "  COMMAND \"${Y60_EXECUTABLE}\" -I \"${PLUGIN_DIRS}\" createTutorialScreenshots.js \"${SCRIPT}\" \"${CMAKE_BINARY_DIR}/${DIRNAME}/TestImages\"\n")
  endif( NOT ${DIRNAME} STREQUAL "tutorials" )
  file(APPEND ${CMAKEFILE} "  WORKING_DIRECTORY \"${TESTMODELDIR}\"\n")
  file(APPEND ${CMAKEFILE} "  RESULT_VARIABLE rv\n")
  file(APPEND ${CMAKEFILE} ")\n")
  file(APPEND ${CMAKEFILE} "if(NOT rv EQUAL 0)\n")
  file(APPEND ${CMAKEFILE} "  message(FATAL_ERROR \"scenetest ${NAME} FAILED\")\n")
  file(APPEND ${CMAKEFILE} "endif(NOT rv EQUAL 0)\n")  
  file(APPEND ${CMAKEFILE} "\n")
  
  #rename file for comparison
  if( ${DIRNAME} STREQUAL "tutorials" )
    file(APPEND ${CMAKEFILE} "configure_file(\"${CMAKE_BINARY_DIR}/${DIRNAME}/TestImages/${NAME}.png\" \"${CMAKE_BINARY_DIR}/${DIRNAME}/TestImages/${NAME}${IMGSUFFIX}\"  COPYONLY) " )
    file(APPEND ${CMAKEFILE} "\n")
  endif( ${DIRNAME} STREQUAL "tutorials" )
    
  set(COUNT 0)
  set(TESTIMG ${CMAKE_BINARY_DIR}/${DIRNAME}/TestImages/${NAME}${IMGSUFFIX}) 
  set(BASELINEIMG ${TESTMODELDIR}/BaselineImages/${NAME}${IMGSUFFIX})
  
  while(EXISTS ${BASELINEIMG})
    file(APPEND ${CMAKEFILE} "#compare image\n")
    file(APPEND ${CMAKEFILE} "if(rv EQUAL 0)\n")
    file(APPEND ${CMAKEFILE} "execute_process(\n")
    file(APPEND ${CMAKEFILE} "  COMMAND \"${COMPAREIMAGE_EXECUTABLE}\" --tolerance ${TOLERANCE} --threshold ${THRESHOLD} \"${TESTIMG}\" \"${BASELINEIMG}\"\n")
    file(APPEND ${CMAKEFILE} "  WORKING_DIRECTORY \"${TESTMODELDIR}\"\n")
    file(APPEND ${CMAKEFILE} "  RESULT_VARIABLE rv\n")
    file(APPEND ${CMAKEFILE} ")\n")
    file(APPEND ${CMAKEFILE} "endif(rv EQUAL 0)\n")  
    file(APPEND ${CMAKEFILE} "\n")
    file(APPEND ${CMAKEFILE} "if(NOT rv EQUAL 0)\n")
    file(APPEND ${CMAKEFILE} "  message(FATAL_ERROR \"scenetest ${NAME} FAILED\")\n")
    file(APPEND ${CMAKEFILE} "endif(NOT rv EQUAL 0)\n")  
    math(EXPR COUNT ${COUNT}+1)
    set(TESTIMG ${CMAKE_BINARY_DIR}/${DIRNAME}/TestImages/${NAME}.${COUNT}${IMGSUFFIX}) 
    set(BASELINEIMG ${TESTMODELDIR}/BaselineImages/${NAME}.${COUNT}${IMGSUFFIX})
  endwhile(EXISTS ${BASELINEIMG})
  
  file(APPEND ${CMAKEFILE} "\n") 

endmacro(write_scenetest )



macro(y60_add_scenetest_custom NAME DIRNAME TOLERANCE THRESHOLD) 
  #generate  ${NAME}.tst.cmake (will include execute_process y60, ac-compare-image)

  set(TESTMODELDIR ${CMAKE_SOURCE_DIR}/../${DIRNAME})
  set(SCRIPT ${TESTMODELDIR}/${NAME}.js)
  if(NOT EXISTS ${SCRIPT})
    set(SCRIPT ${CMAKE_SOURCE_DIR}/y60/js/scenetest.js)
  endif(NOT EXISTS ${SCRIPT})
  set(SCENE ${TESTMODELDIR}/${NAME}.x60)
  if(NOT EXISTS ${SCENE})
    set(SCENE_H ${TESTMODELDIR}/${NAME}.b60)
    if(NOT EXISTS ${SCENE_H})
      set(SCENE_H "")
    endif(NOT EXISTS ${SCENE_H})  
    set(SCENE ${SCENE_H})
  endif(NOT EXISTS ${SCENE})
  set(SHADERLIB ${CMAKE_SOURCE_DIR}/y60/shader/shaderlibrary_nocg.xml)
  set(IMGSUFFIX .nocg.png)
  set(ONEVERSION 0)
  set(CMAKEFILE ${CMAKE_BINARY_DIR}/${DIRNAME}/${NAME}.tst.cmake)
  set(CMAKEFILECG ${CMAKE_BINARY_DIR}/${DIRNAME}/${NAME}.cg.tst.cmake)
  
  if(NOT EXISTS ${CMAKE_BINARY_DIR}/${DIRNAME}/TestImages)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${DIRNAME}/TestImages)
  endif(NOT EXISTS ${CMAKE_BINARY_DIR}/${DIRNAME}/TestImages)
  
  # generate no cg test (if available)
  if(EXISTS ${TESTMODELDIR}/BaselineImages/${NAME}${IMGSUFFIX})
    set(ONEVERSION 1)
    write_scenetest( ${NAME} ${CMAKEFILE} ${DIRNAME} 0 ${TOLERANCE} ${THRESHOLD})
        
    # register the created cmake-file as test
    add_test(${DIRNAME}_${NAME}
      cmake -P ${CMAKEFILE}
    )
  endif(EXISTS ${TESTMODELDIR}/BaselineImages/${NAME}${IMGSUFFIX})
    
  # generate no cg test (if available)
  set(SHADERLIB ${CMAKE_SOURCE_DIR}/y60/shader/shaderlibrary.xml)
  set(IMGSUFFIX .cg.png)
  if(EXISTS ${TESTMODELDIR}/BaselineImages/${NAME}${IMGSUFFIX})
    set(ONEVERSION 1)
    write_scenetest( ${NAME} ${CMAKEFILECG} ${DIRNAME} 1 ${TOLERANCE} ${THRESHOLD})
  
    # register the created cmake-file as test
    add_test(${DIRNAME}_CG_${NAME}
      cmake -P ${CMAKEFILECG}
    )
  endif(EXISTS ${TESTMODELDIR}/BaselineImages/${NAME}${IMGSUFFIX})

  if(${ONEVERSION} EQUAL 0)  
    message("SKIPPED. (No BaselineImages found for scenetest ${NAME}.)")
  endif(${ONEVERSION} EQUAL 0)  
  
endmacro(y60_add_scenetest_custom)

macro(y60_add_scenetest NAME DIRNAME)
    y60_add_scenetest_custom(${NAME} ${DIRNAME} "0.12" "30") 
endmacro(y60_add_scenetest NAME DIRNAME)  
