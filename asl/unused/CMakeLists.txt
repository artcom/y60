
set(SOURCES
)

set(HEADERS
#GarbageCollector.h
)

set(TESTS
#GarbageCollector
)

if(HEADERS OR SOURCES)
    ac_add_library(
        aslunused
        DEPENDS aslbase
        SOURCES ${SOURCES}
        HEADERS ${HEADERS}
        TESTS   ${TESTS}
    )
endif(HEADERS OR SOURCES)

