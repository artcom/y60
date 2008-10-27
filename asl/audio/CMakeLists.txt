
set(SOURCES
AudioBufferBase.cpp
AudioBuffer.cpp
AudioTimeSource.cpp
DummyPump.cpp
Effect.cpp
ExternalTimeSource.cpp
HWSampleSink.cpp
Pump.cpp
SampleFormat.cpp
SampleSource.cpp
TestPump.cpp
VolumeFader.cpp
)

set(HEADERS
AudioBufferBase.h
AudioBuffer.h
AudioException.h
AudioTimeSource.h
DummyPump.h
Effect.h
ExternalTimeSource.h
HWSampleSink.h
ISampleSink.h
Pump.h
SampleFormat.h
SampleSource.h
TestPump.h
TimeSource.h
VolumeFader.h
)

set(TESTS
DummyPump
Effect
RealPump
)

set(EXTERNS)

if(APPLE)
    set(SOURCES ${SOURCES} CoreAudioPump.cpp)
    set(HEADERS ${HEADERS} CoreAudioPump.h)
endif(APPLE)

if(LINUX)
    list(APPEND EXTERNS ALSA)
    set(SOURCES ${SOURCES} ALSAPump.cpp)
    set(HEADERS ${HEADERS} ALSAPump.h)
endif(LINUX)

if(WIN32)
    set(SOURCES ${SOURCES} DirectSoundPump.cpp)
    set(HEADERS ${HEADERS} DirectSoundPump.h DSMessages.h)
endif(WIN32)

ac_add_library(
    aslaudio
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS ${EXTERNS}
    DEPENDS aslbase
    TESTS ${TESTS}
)
