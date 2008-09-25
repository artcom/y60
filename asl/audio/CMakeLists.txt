
set(SOURCE
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

if(APPLE)
  set(SOURCE ${SOURCE} CoreAudioPump.cpp)
  set(HEADERS ${HEADERS} CoreAudioPump.h)
endif(APPLE)

if(LINUX)
  set(SOURCE ${SOURCE} ALSAPump.cpp)
  set(HEADERS ${HEADERS} ALSAPump.h)
endif(LINUX)

if(WIN32)
  set(SOURCE ${SOURCE} DirectSoundPump.cpp)
  set(HEADERS ${HEADERS} DirectSoundPump.h DSMessages.h)
endif(WIN32)

add_library(aslaudio SHARED ${SOURCE})
set_target_properties(aslaudio PROPERTIES PUBLIC_HEADER "${HEADERS}")
target_link_libraries(aslaudio aslbase aslmath )

if(LINUX)
  link_directories(aslaudio ${ALSA_INCLUDE_DIRECTORIES})
  include_directories(aslaudio ${ALSA_LIBRARY_DIRECTORIES})
  target_link_libraries(aslaudio ${ALSA_LIBRARIES})
endif(LINUX)

install(
  TARGETS aslaudio
  LIBRARY
    DESTINATION lib
  PUBLIC_HEADER 
    DESTINATION include/asl/audio
)

foreach(test ${TESTS})
  add_executable(test${test} test${test}.tst.cpp)
  target_link_libraries(test${test} aslaudio)
  add_test(${test} test${test})
endforeach(test)
