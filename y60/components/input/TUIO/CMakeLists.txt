
set(SOURCES
TUIOPlugin.cpp
tuioeventxsd.cpp
)

set(SERVER_SOURCES
TUIOServerPlugin.cpp
)

set(HEADERS
tuioeventxsd.h
)

set(EXTERNS
TUIO
)

find_file(
    TUIO_BLOB_DIR
    TuioBlob.h
    PATH_SUFFIXES TUIO
)
if(TUIO_BLOB_DIR)
add_definitions(-DTUIO_API_VERSION_1_5)
else(TUIO_BLOB_DIR)
add_definitions(-DTUIO_API_VERSION_1_4)
endif(TUIO_BLOB_DIR)

if(WIN32)
    list(APPEND EXTERNS WINMM)
endif(WIN32)

y60_add_component(
    TUIOClient
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS ${EXTERNS}
)

y60_add_component(
    TUIOServer
    SOURCES ${SERVER_SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS ${EXTERNS}
)

asl_add_schema(
    tuioevent.xsd
    CXX_FILE      tuioeventxsd.cpp
    CXX_NAMESPACE y60
    CXX_VARNAME   ourtuioeventxsd
    CXX_INCLUDE   tuioeventxsd.h
)

