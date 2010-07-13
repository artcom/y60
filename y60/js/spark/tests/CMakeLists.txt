set(JSTESTS
Common
Meta
Loader
Rectangle
)

set(NOCG_TESTS
testCanvas
)

foreach(TEST ${JSTESTS})
  y60_add_jstest(${TEST} spark)
endforeach(TEST)

make_directory(${CMAKE_CURRENT_BINARY_DIR}/TestImages)

function(add_rendertest TEST SUFFIX SHADERLIBRARY)
  set(LOAD)

  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TEST}.js)
      list(APPEND LOAD ${TEST}.js)
  else(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TEST}.js)
      list(APPEND LOAD scenetest.js)
  endif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TEST}.js)

  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TEST}.x60)
      list(APPEND LOAD ${TEST}.x60)
  endif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TEST}.x60)

  list(APPEND LOAD ${SHADERLIBRARY})

  y60_add_rendertest(
      ${TEST}_${SUFFIX}
      LOAD ${LOAD}

      OUTPUT_DIR   ${CMAKE_CURRENT_BINARY_DIR}/TestImages
      BASELINE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/BaselineImages

      IMAGE_PREFIX ${TEST}
      IMAGE_SUFFIX ${SUFFIX}
  )
endfunction(add_rendertest)

foreach(TEST ${CG_TESTS})
  add_rendertest(${TEST} cg shaderlibrary.xml)
endforeach(TEST)

foreach(TEST ${NOCG_TESTS})
  add_rendertest(${TEST} nocg shaderlibrary_nocg.xml)
endforeach(TEST)