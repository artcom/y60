
set(SOURCES
AlliedEthernetCameraCapture.cpp 
)

set(HEADERS
AlliedEthernetCameraCapture.h
)

set(EXTERNALS FFMPEG  ALLIED_GIGABIT)

y60_add_component(
    AlliedEthernetCameraCapture
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60video y60jslib
    EXTERNS ${EXTERNALS}
)
