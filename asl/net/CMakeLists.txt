
set(SOURCES
address.cpp
INetEndpoint.cpp
INetMonitor.cpp
net.cpp
net_functions.cpp
Socket.cpp
SocketException.cpp
Station.cpp
TCPClientSocket.cpp
TCPServer.cpp
TCPSocket.cpp
TestINetMonitor.cpp
TestSocket.cpp
UDPConnection.cpp
UDPSocket.cpp
)

set(HEADERS
address.h
asl_net_settings.h
INetEndpoint.h
INetMonitor.h
net_functions.h
net.h
SocketException.h
Socket.h
Station.h
TCPClientSocket.h
TCPServer.h
TCPSocket.h
TestINetMonitor.h
TestSocket.h
UDPConnection.h
UDPSocket.h
)

set(TESTS
INet
_net_functions
Station
)

list(APPEND EXTERNS CURL ZLIB) # Station uses zlib

if(WIN32)
    list(APPEND EXTERNS WINSOCK)
endif(WIN32)

ac_add_library(
    aslnet asl/net
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    TESTS   ${TESTS}
    DEPENDS aslbase
    EXTERNS ${EXTERNS}
)
