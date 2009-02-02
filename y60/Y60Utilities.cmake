# Some utilities for building Y60-related things

include(AcMake)

macro(y60_add_component COMPONENT_NAME)
    ac_add_plugin(
        y60${COMPONENT_NAME} y60/components
        ${ARGN}
    )
endmacro(y60_add_component)


macro(y60_add_jstest NAME )
  get_target_property(Y60_EXECUTABLE y60 LOCATION_${CMAKE_BUILD_TYPE}) 
  set(TESTPATH ${CMAKE_CURRENT_BINARY_DIR})
  string(REPLACE "${CMAKE_BINARY_DIR}" "${CMAKE_SOURCE_DIR}" TESTPATH ${TESTPATH})
  add_test(${NAME}
      ${Y60_EXECUTABLE} test${NAME}.tst.js ${ARGN} shaderlibrary.xml -I 
      "${TESTPATH};${CMAKE_BINARY_DIR}/lib/Release;${CMAKE_SOURCE_DIR}/y60/products/y60/;${CMAKE_SOURCE_DIR}/y60/js/;${CMAKE_SOURCE_DIR}/y60/shader/;."
  )
endmacro(y60_add_jstest ) 


