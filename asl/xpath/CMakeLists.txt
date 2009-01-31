
set(SOURCES
Expression.cpp
PathParser.cpp
XPathPath.cpp
XPathValue.cpp
xpath_api.cpp
)

set(HEADERS
asl_xpath_settings.h
Expression.h
ParseInput.h
XPathPath.h
PathParser.h
XPathValue.h
xpath_api.h
xpath_api_impl.h
)

set(TESTS
XPath
)

ac_add_library(
    aslxpath asl/xpath
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    TESTS   ${TESTS}
    DEPENDS aslbase asldom aslraster
)
