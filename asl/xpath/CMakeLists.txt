
set(SOURCE
Expression.cpp
Path.cpp
PathParser.cpp
Value.cpp
xpath_api.cpp
)

set(HEADERS
Expression.h
ParseInput.h
Path.h
PathParser.h
Value.h
xpath_api.h
xpath_api_impl.h
)

set(TESTS
XPath
)

if(APPLE)
  set(SOURCE ${SOURCE})
  set(HEADERS ${HEADERS})
endif(APPLE)

if(LINUX)
  set(SOURCE ${SOURCE})
  set(HEADERS ${HEADERS})
endif(LINUX)

if(WIN32)
  set(SOURCE ${SOURCE})
  set(HEADERS ${HEADERS})
endif(WIN32)

add_library(aslxpath SHARED ${SOURCE})
set_target_properties(aslxpath PROPERTIES PUBLIC_HEADER "${HEADERS}")
target_link_libraries(aslxpath aslbase asldom)

install(
  TARGETS aslxpath
  LIBRARY
    DESTINATION lib
  PUBLIC_HEADER 
    DESTINATION include/asl/xpath
)

foreach(test ${TESTS})
  add_executable(test${test} test${test}.tst.cpp)
  target_link_libraries(test${test} aslxpath)
  add_test(${test} test${test})
endforeach(test)

