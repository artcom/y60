
set(SOURCE
CRC8.cpp
SerialDevice.cpp
SerialDeviceFactory.cpp
TTYPort.cpp
uCCP.cpp
)

set(HEADERS
CRC8.h
DebugPort.h
SerialDeviceFactory.h
SerialDevice.h
TTYPort.h
uCCP.h
)

set(TESTS
SerialDevice
)

if(APPLE)
  set(SOURCE ${SOURCE})
  set(HEADERS ${HEADERS})
endif(APPLE)

if(LINUX)
  set(SOURCE ${SOURCE})
  set(HEADERS ${HEADERS})
endif(LINUX)

if(WIN32)
  set(SOURCE ${SOURCE} ComPort.cpp)
  set(HEADERS ${HEADERS} ComPort.h)
endif(WIN32)

add_library(aslserial SHARED ${SOURCE})
set_target_properties(aslserial PROPERTIES PUBLIC_HEADER "${HEADERS}")
target_link_libraries(aslserial aslbase aslmath)

install(
  TARGETS aslserial
  LIBRARY
    DESTINATION lib
  PUBLIC_HEADER 
    DESTINATION include/asl/serial
)

foreach(test ${TESTS})
  add_executable(test${test} test${test}.tst.cpp)
  target_link_libraries(test${test} aslserial)
  add_test(${test} test${test})
endforeach(test)

