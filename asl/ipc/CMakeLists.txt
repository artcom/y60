
set(SOURCE
SocketPolicy.cpp
TCPPolicy.cpp
UnixSocketPolicy.cpp
)

set(HEADERS
ConduitAcceptor.h
Conduit.h
ConduitPolicy.h
ConduitServer.h
LocalPolicy.h
MessageAcceptor.h
MessageServer.h
SocketPolicy.h
TCPPolicy.h
ThreadedConduit.h
UnixSocketPolicy.h
)

set(TESTS
#Conduit
)

if(WIN32)
  set(SOURCE ${SOURCE} NamedPipePolicy.cpp)
  set(HEADERS ${HEADERS} NamedPipePolicy.h)
endif(WIN32)

add_library(aslipc SHARED ${SOURCE})
set_target_properties(aslipc PROPERTIES PUBLIC_HEADER "${HEADERS}")
target_link_libraries(aslipc aslbase aslnet)

install(
  TARGETS aslipc
  LIBRARY
    DESTINATION lib
  PUBLIC_HEADER 
    DESTINATION include/asl/ipc
)

foreach(test ${TESTS})
  add_executable(test${test} test${test}.tst.cpp)
  target_link_libraries(test${test} aslipc)
  add_test(${test} test${test})
endforeach(test)

