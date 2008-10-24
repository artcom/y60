
set(SOURCES
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
  set(SOURCES ${SOURCES} NamedPipePolicy.cpp)
  set(HEADERS ${HEADERS} NamedPipePolicy.h)
endif(WIN32)

ac_add_library(
  ipc
  SOURCES ${SOURCES}
  HEADERS ${HEADERS}
  TESTS ${TESTS}
  DEPENDS aslbase aslnet
)
