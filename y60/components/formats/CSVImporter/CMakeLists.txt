
set(SOURCES
CSVImporter.cpp 
JSCSVImporter.cpp 
JSCSVPlugin.cpp 
)

set(HEADERS
CSVImporter.h
JSCSVImporter.h
y60_csvimporter_settings.h
)

y60_add_component(
    CSV
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib aslcsv ${JSDEPENDS}
    EXTERNS ${JSEXTERN} GLEW 
)

