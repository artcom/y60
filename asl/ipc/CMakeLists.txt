
set(SOURCES
SocketPolicy.cpp
TCPPolicy.cpp
)

set(HEADERS
asl_ipc_settings.h
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
)

set(TESTS
#Conduit
)

if(WIN32)
    list(APPEND SOURCES NamedPipePolicy.cpp)
    list(APPEND HEADERS NamedPipePolicy.h)
	list( APPEND EXTERNALS PTHREADVC )
else(WIN32)
    list(APPEND SOURCES UnixSocketPolicy.cpp)
    list(APPEND HEADERS UnixSocketPolicy.h)
endif(WIN32)

ac_add_library(
    aslipc asl/ipc
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    TESTS   ${TESTS}
    EXTERNS ${EXTERNALS}
    DEPENDS aslbase aslnet
)
