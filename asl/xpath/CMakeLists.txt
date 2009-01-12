
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

list( APPEND EXTERNALS OPENEXR )

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

ac_add_library(
    aslxpath asl/xpath
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    TESTS   ${TESTS}
    DEPENDS aslbase asldom aslraster
    EXTERNS ${EXTERNALS}
)
