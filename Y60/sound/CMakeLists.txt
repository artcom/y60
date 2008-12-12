
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
)

set(TESTS
DummySound
GrainSource
RealSound
)

if(OSX)
endif(OSX)

if(LINUX)
endif(LINUX)

if(WIN32)
endif(WIN32)

ac_add_library(
    y60sound y60/sound
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase aslmath aslaudio
    EXTERNS AVFORMAT
    TESTS   ${TESTS}
)

