
set(SOURCES
AlliedEthernetCameraCapture.cpp 
)

set(HEADERS
AlliedEthernetCameraCapture.h
)

set(EXTERNALS FFMPEG  ALLIED_GIGABIT)

add_definitions(-D__STDC_CONSTANT_MACROS) # ffmpeg is c99 only and needs some c macros

y60_add_component(
    AlliedEthernetCameraCapture
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60video y60jslib
    EXTERNS ${EXTERNALS}
)
