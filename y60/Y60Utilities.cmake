# Some utilities for building Y60-related things

include(AcMake)

macro(y60_maybe_install_directory LOCAL_NAME INSTALL_NAME)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIRECTORY}/${LOCAL_NAME})
        install(
            DIRECTORY ${CMAKE_CURRENT_SOURCE_DIRECTORY}/${LOCAL_NAME}/
            DESTINATION ${INSTALL_NAME}
            FILES_MATCHING PATTERN ".svn" EXCLUDE
        )
    endif(EXISTS ${CMAKE_CURRENT_SOURCE_DIRECTORY}/${LOCAL_NAME})
endmacro(y60_maybe_install_directory LOCAL_NAME INSTALL_NAME)

macro(y60_add_application APPLICATION_NAME)
    parse_arguments(
        THIS_APPLICATION
        ""
        ""
        ${ARGN}
    )

    y60_maybe_install_directory(CONFIG etc/${APPLICATION_NAME})

    y60_maybe_install_directory(SCRIPTS lib/${APPLICATION_NAME}/js)
    y60_maybe_install_directory(TEX     lib/${APPLICATION_NAME}/textures)
    y60_maybe_install_directory(FONTS   lib/${APPLICATION_NAME}/fonts)
    y60_maybe_install_directory(MODELS  lib/${APPLICATION_NAME}/models)
    y60_maybe_install_directory(SOUNDS  lib/${APPLICATION_NAME}/sounds)
    y60_maybe_install_directory(MOVIES  lib/${APPLICATION_NAME}/movies)

endmacro(y60_add_application)

macro(y60_add_component COMPONENT_NAME)
    ac_add_plugin(
        y60${COMPONENT_NAME} y60/components
        ${ARGN}
    )
endmacro(y60_add_component)


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
  get_target_property(COMPAREIMAGE_EXECUTABLE ac-compare-image LOCATION_${CMAKE_BUILD_TYPE})

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
    file(APPEND ${CMAKEFILE} "include(${TESTMODELDIR}/${NAME}.pre.cmake)\n")
    file(APPEND ${CMAKEFILE} "${NAME}_pre(${CMAKE_BINARY_DIR} ${TESTMODELDIR}) \n\n")
  endif(EXISTS ${TESTMODELDIR}/${NAME}.pre.cmake)

  file(APPEND ${CMAKEFILE} "#generate image\n")
  file(APPEND ${CMAKEFILE} "execute_process(\n")
  if( NOT ${DIRNAME} STREQUAL "tutorials" )
    file(APPEND ${CMAKEFILE} "  COMMAND ${Y60_EXECUTABLE} -I \"${PLUGIN_DIRS}\" ${SCRIPT} ${SCENE} ${SHADERLIB} rehearsal offscreen outputimage=${CMAKE_BINARY_DIR}/${DIRNAME}/TestImages/${NAME} outputsuffix=${IMGSUFFIX}\n")
  else( NOT ${DIRNAME} STREQUAL "tutorials" )
    file(APPEND ${CMAKEFILE} "  COMMAND ${Y60_EXECUTABLE} -I \"${PLUGIN_DIRS}\" createTutorialScreenshots.js ${SCRIPT} ${CMAKE_BINARY_DIR}/${DIRNAME}/TestImages\n")
  endif( NOT ${DIRNAME} STREQUAL "tutorials" )
  file(APPEND ${CMAKEFILE} "  WORKING_DIRECTORY ${TESTMODELDIR}\n")
  file(APPEND ${CMAKEFILE} "  RESULT_VARIABLE rv\n")
  file(APPEND ${CMAKEFILE} ")\n")
  file(APPEND ${CMAKEFILE} "if(NOT rv EQUAL 0)\n")
  file(APPEND ${CMAKEFILE} "  message(FATAL_ERROR \"scenetest ${NAME} FAILED\")\n")
  file(APPEND ${CMAKEFILE} "endif(NOT rv EQUAL 0)\n")  
  file(APPEND ${CMAKEFILE} "\n")
  
  #rename file for comparison
  if( ${DIRNAME} STREQUAL "tutorials" )
    file(APPEND ${CMAKEFILE} "configure_file(${CMAKE_BINARY_DIR}/${DIRNAME}/TestImages/${NAME}.png ${CMAKE_BINARY_DIR}/${DIRNAME}/TestImages/${NAME}${IMGSUFFIX}  COPYONLY) " )
    file(APPEND ${CMAKEFILE} "\n")
  endif( ${DIRNAME} STREQUAL "tutorials" )
    
  set(COUNT 0)
  set(TESTIMG ${CMAKE_BINARY_DIR}/${DIRNAME}/TestImages/${NAME}${IMGSUFFIX}) 
  set(BASELINEIMG ${TESTMODELDIR}/BaselineImages/${NAME}${IMGSUFFIX})
  
  while(EXISTS ${BASELINEIMG})
    file(APPEND ${CMAKEFILE} "#compare image\n")
    file(APPEND ${CMAKEFILE} "if(rv EQUAL 0)\n")
    file(APPEND ${CMAKEFILE} "execute_process(\n")
    file(APPEND ${CMAKEFILE} "  COMMAND ${COMPAREIMAGE_EXECUTABLE} --tolerance ${TOLERANCE} --threshold ${THRESHOLD} ${TESTIMG} ${BASELINEIMG}\n")
    file(APPEND ${CMAKEFILE} "  WORKING_DIRECTORY ${TESTMODELDIR}\n")
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
