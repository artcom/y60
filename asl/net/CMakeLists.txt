
set(SOURCE
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

add_library(aslnet SHARED ${SOURCE})
set_target_properties(aslnet PROPERTIES PUBLIC_HEADER "${HEADERS}")
link_directories(aslnet ${CURL_LIBRARY_DIRECTORIES})
include_directories(aslnet ${CURL_INCLUDE_DIRECTORIES})
target_link_libraries(aslnet aslbase aslmath ${CURL_LIBRARIES})

install(
  TARGETS aslnet
  LIBRARY
    DESTINATION lib
  PUBLIC_HEADER 
    DESTINATION include/asl/net
)

foreach(test ${TESTS})
  add_executable(test${test} test${test}.tst.cpp)
  target_link_libraries(test${test} aslnet)
  add_test(${test} test${test})
endforeach(test)

