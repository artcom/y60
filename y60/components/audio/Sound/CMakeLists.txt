
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

y60_add_component(
    Sound
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib y60sound
)

y60_add_jstest(Audio mod_JSSound)

