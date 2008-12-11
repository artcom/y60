
set(SOURCES
Expression.cpp
PathParser.cpp
XPathPath.cpp
XPathValue.cpp
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
    aslxpath
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    TESTS ${TESTS}
    DEPENDS aslbase asldom aslraster
    EXTERNS OPEN_EXR
)
