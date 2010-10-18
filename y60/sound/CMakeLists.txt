
set(SOURCES
CacheReader.cpp
FFMpegAudioDecoder.cpp
GrainSource.cpp
Sound.cpp
SoundCacheItem.cpp
SoundManager.cpp
WindowFunction.cpp
)

set(HEADERS
CacheReader.h
FFMpegAudioDecoder.h
GrainSource.h
IAudioDecoder.h
IAudioDecoderFactory.h
Resampler.h
Sound.h
SoundCacheItem.h
SoundManager.h
TestGrainSource.h
TestSound.h
WindowFunction.h
y60_sound_settings.h
)

set(TESTS
DummySound
GrainSource
RealSound
)

list(APPEND EXTERNALS FFMPEG)

add_definitions(-D__STDC_CONSTANT_MACROS) # ffmpeg is c99 only and needs some c macros

ac_add_library(
    y60sound y60/sound
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase aslmath aslaudio
    EXTERNS ${EXTERNALS}
    TESTS   ${TESTS}
)
