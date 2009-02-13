# Some utilities for building Y60-related things

include(AcMake)

macro(y60_add_component COMPONENT_NAME)
    ac_add_plugin(
        y60${COMPONENT_NAME} y60/components
        ${ARGN}
    )
endmacro(y60_add_component)


macro(collect_plugin_dirs PLUGIN_DIRS) 
  get_property(Y60_PLUGINS GLOBAL PROPERTY Y60_PLUGINS)
  set(PLUGIN_DIRS "")
  foreach(PLUGIN_NAME ${Y60_PLUGINS})
    get_target_property(PLUGIN_FILE ${PLUGIN_NAME} LOCATION_${CMAKE_BUILD_TYPE}) 
    get_filename_component(PLUGIN_DIR ${PLUGIN_FILE} PATH)
    list(APPEND PLUGIN_DIRS ${PLUGIN_DIR})
  endforeach(PLUGIN_NAME ${Y60_PLUGINS})
  list(REMOVE_DUPLICATES PLUGIN_DIRS)
endmacro(collect_plugin_dirs) 

macro(y60_add_jstest NAME )
  get_target_property(Y60_EXECUTABLE y60 LOCATION_${CMAKE_BUILD_TYPE}) 
  collect_plugin_dirs(PLUGIN_DIRS) 
  set(TESTPATH ${CMAKE_CURRENT_BINARY_DIR})
  string(REPLACE "${CMAKE_BINARY_DIR}" "${CMAKE_SOURCE_DIR}" TESTPATH ${TESTPATH})
  add_test(jstest_${NAME}
      ${Y60_EXECUTABLE} test${NAME}.tst.js ${ARGN} shaderlibrary.xml -I 
      "${TESTPATH};${CMAKE_BINARY_DIR}/lib/Release;${CMAKE_SOURCE_DIR}/y60/products/y60/;${CMAKE_SOURCE_DIR}/y60/js/;${CMAKE_SOURCE_DIR}/y60/shader/;${PLUGIN_DIRS};."
  )
endmacro(y60_add_jstest )

macro(write_scenetest NAME CMAKEFILE)
  get_target_property(Y60_EXECUTABLE y60 LOCATION_${CMAKE_BUILD_TYPE}) 
  get_target_property(COMPAREIMAGE_EXECUTABLE ac-compare-image LOCATION_${CMAKE_BUILD_TYPE})

  collect_plugin_dirs(PLUGIN_DIRS) 

  file(APPEND ${CMAKEFILE} "#generate image\n")
  file(APPEND ${CMAKEFILE} "execute_process(\n")
  file(APPEND ${CMAKEFILE} "  COMMAND ${Y60_EXECUTABLE} -I \"${CMAKE_BINARY_DIR}/testmodels;${CMAKE_SOURCE_DIR}/y60/js;${PLUGIN_DIRS};${CMAKE_SOURCE_DIR}/y60/shader\" ${SCRIPT} ${SCENE} ${SHADERLIB} rehearsal offscreen outputimage=${CMAKE_BINARY_DIR}/testmodels/TestImages/${NAME} outputsuffix=${IMGSUFFIX}\n")
  file(APPEND ${CMAKEFILE} "  WORKING_DIRECTORY ${TESTMODELDIR}\n")
  file(APPEND ${CMAKEFILE} "  RESULT_VARIABLE rv\n")
  file(APPEND ${CMAKEFILE} ")\n")
  file(APPEND ${CMAKEFILE} "if(NOT rv EQUAL 0)\n")
  file(APPEND ${CMAKEFILE} "  message(FATAL_ERROR \"scenetest ${NAME} FAILED\")\n")
  file(APPEND ${CMAKEFILE} "endif(NOT rv EQUAL 0)\n")  
  file(APPEND ${CMAKEFILE} "\n")
  
  set(COUNT 0)
  set(TESTIMG ${CMAKE_BINARY_DIR}/testmodels/TestImages/${NAME}${IMGSUFFIX}) 
  set(BASELINEIMG ${TESTMODELDIR}/BaselineImages/${NAME}${IMGSUFFIX})
  
  while(EXISTS ${BASELINEIMG})
    file(APPEND ${CMAKEFILE} "#compare image\n")
    file(APPEND ${CMAKEFILE} "if(rv EQUAL 0)\n")
    file(APPEND ${CMAKEFILE} "execute_process(\n")
    file(APPEND ${CMAKEFILE} "  COMMAND ${COMPAREIMAGE_EXECUTABLE} --tolerance 0.12 --threshold 30 ${TESTIMG} ${BASELINEIMG}\n")
    file(APPEND ${CMAKEFILE} "  WORKING_DIRECTORY ${TESTMODELDIR}\n")
    file(APPEND ${CMAKEFILE} "  RESULT_VARIABLE rv\n")
    file(APPEND ${CMAKEFILE} ")\n")
    file(APPEND ${CMAKEFILE} "endif(rv EQUAL 0)\n")  
    file(APPEND ${CMAKEFILE} "\n")
    file(APPEND ${CMAKEFILE} "if(NOT rv EQUAL 0)\n")
    file(APPEND ${CMAKEFILE} "  message(FATAL_ERROR \"scenetest ${NAME} FAILED\")\n")
    file(APPEND ${CMAKEFILE} "endif(NOT rv EQUAL 0)\n")  
    math(EXPR COUNT ${COUNT}+1)
    set(TESTIMG ${CMAKE_BINARY_DIR}/testmodels/TestImages/${NAME}.${COUNT}${IMGSUFFIX}) 
    set(BASELINEIMG ${TESTMODELDIR}/BaselineImages/${NAME}.${COUNT}${IMGSUFFIX})
  endwhile(EXISTS ${BASELINEIMG})
  
endmacro(write_scenetest )

macro(y60_add_scenetest NAME ) 

  #generate  ${NAME}.tst.cmake (will include execute_process y60, ac-compare-image) 
  set(TESTMODELDIR ${CMAKE_SOURCE_DIR}/../testmodels)
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
  set(CMAKEFILE ${CMAKE_BINARY_DIR}/testmodels/${NAME}.tst.cmake)
  
  if(NOT EXISTS ${CMAKE_BINARY_DIR}/testmodels/TestImages)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/testmodels/TestImages)
  endif(NOT EXISTS ${CMAKE_BINARY_DIR}/testmodels/TestImages)
  
  file(WRITE ${CMAKEFILE} "# testscript for scenetest ${NAME}\n")
  file(APPEND ${CMAKEFILE} "#   will be called with cmake -P ${CMAKEFILE}\n\n")
  if(EXISTS ${TESTMODELDIR}/${NAME}.pre.cmake)
    file(APPEND ${CMAKEFILE} "include(${TESTMODELDIR}/${NAME}.pre.cmake)\n")
    file(APPEND ${CMAKEFILE} "${NAME}_pre(${CMAKE_BINARY_DIR} ${TESTMODELDIR}) \n\n")
  endif(EXISTS ${TESTMODELDIR}/${NAME}.pre.cmake)
  
  if(EXISTS ${TESTMODELDIR}/BaselineImages/${NAME}${IMGSUFFIX})
    set(ONEVERSION 1)
    file(APPEND ${CMAKEFILE} "message(\"*********************** NO CG *********************\")\n")
    write_scenetest( ${NAME} ${CMAKEFILE})
  endif(EXISTS ${TESTMODELDIR}/BaselineImages/${NAME}${IMGSUFFIX})
  set(SHADERLIB ${CMAKE_SOURCE_DIR}/y60/shader/shaderlibrary.xml)
  set(IMGSUFFIX .cg.png)
  if(EXISTS ${TESTMODELDIR}/BaselineImages/${NAME}${IMGSUFFIX})
    set(ONEVERSION 1)
    file(APPEND ${CMAKEFILE} "\n\nmessage(\"*********************** CG *********************\")\n")
    write_scenetest( ${NAME} ${CMAKEFILE})
  endif(EXISTS ${TESTMODELDIR}/BaselineImages/${NAME}${IMGSUFFIX})

  if(${ONEVERSION} EQUAL 0)  
    file(APPEND ${CMAKEFILE} "  message( \"SKIPPED. (No BaselineImages found for scenetest ${NAME}.)\")\n")
  endif(${ONEVERSION} EQUAL 0)  
  file(APPEND ${CMAKEFILE} "\n") 
  
  # register the created cmake-file as test
  add_test(scenetest_${NAME}
    cmake -P ${CMAKEFILE}
  )
endmacro(y60_add_scenetest ) 
