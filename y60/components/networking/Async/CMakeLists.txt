set(SOURCES
Connection.cpp
HttpServer.cpp
curl/Client.cpp
JSHttpClient.cpp
JSHttpServer.cpp
JSWebSocketClient.cpp
NetAsync.cpp
curl/SocketAdapter.cpp
curl/MultiAdapter.cpp
websocket/Client.cpp
websocket/Manager.cpp
MimeTypes.cpp
Reply.cpp
RequestParser.cpp
)

set(HEADERS
ConcurrentQueue.h
Connection.h
HttpHeader.h
HttpServer.h
curl/Client.h
JSHttpClient.h
# JSWebSocketClient.h
JSHttpServer.h
MimeTypes.h
NetAsync.h
curl/SocketAdapter.h
curl/MultiAdapter.h
Reply.h
Request.h
RequestParser.h
y60_netasync_settings.h
)

find_package(OpenSSL)
list( APPEND EXTERNALS OPENSSL )


if(NOT WIN32)
    find_package(Boost COMPONENTS thread system regex)
endif(NOT WIN32)

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

y60_add_component(
    NetAsync
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS asldom aslnet spidermonkey y60jsbase y60jslib
    EXTERNS ${EXTERNALS} 
)

y60_add_jstest(HttpServer mod_Async)
y60_add_jstest(HttpClient mod_Async)

