
set(SOURCES
JSCSVImporter.cpp
)

set(HEADERS
JSCSVImporter.h
y60_csvimporter_settings.h
)

y60_add_component(
    CSVImporter
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib aslcsv ${JSDEPENDS}
    EXTERNS ${JSEXTERN} GLEW 
)

