
set(SOURCES
address.cpp
Hop.cpp
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
Hop.h
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

ac_add_library(
    aslnet
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    TESTS ${TESTS}
    DEPENDS aslbase
    EXTERNS CURL
)

