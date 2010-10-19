set(SOURCES
CSVParser.cpp
)
set(HEADERS
CSVParser.h
)
set(TESTS
CSVParser
)

set(EXTERNS Boost)

ac_add_library(
    aslcsv asl/csv
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS ${EXTERNS}
    DEPENDS aslbase aslzip
    TESTS   ${TESTS}
)

