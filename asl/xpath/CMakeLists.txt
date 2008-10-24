
set(SOURCES
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

ac_add_library(
  xpath
  SOURCES ${SOURCES}
  HEADERS ${HEADERS}
  TESTS ${TESTS}
  DEPENDS aslbase asldom
)
