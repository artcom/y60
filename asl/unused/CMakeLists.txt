
set(SOURCES
)

set(HEADERS
)

if(HEADERS OR SOURCES)
    ac_add_library(
        aslunused
        SOURCES ${SOURCES}
        HEADERS ${HEADERS}
    )
endif(HEADERS OR SOURCES)

