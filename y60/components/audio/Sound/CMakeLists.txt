
set(SOURCES
JSGrainSource.cpp
JSSound.cpp
JSSoundManager.cpp
)

set(HEADERS
JSGrainSource.h
JSSound.h
JSSoundManager.h
y60_jssound_settings.h
)

add_definitions(-D__STDC_CONSTANT_MACROS) # ffmpeg is c99 only and needs some c macros

y60_add_component(
    Sound
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib y60sound
)

y60_add_jstest(Audio mod_JSSound)

