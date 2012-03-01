set(SOURCES
Connection.cpp
HttpServer.cpp
curl/Client.cpp
JSHttpClient.cpp
JSHttpServer.cpp
NetAsync.cpp
curl/SocketAdapter.cpp
curl/MultiAdapter.cpp
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

list ( APPEND EXTERNALS )

if(NOT WIN32)
    find_package(Boost COMPONENTS thread system)
endif(NOT WIN32)

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

y60_add_component(
    NetAsync
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS asldom aslnet spidermonkey y60jsbase y60jslib
    EXTERNS ${EXTERNALS} Boost 
)

# XXX Hangs (occasionally) on windows
# y60_add_jstest(HttpServer mod_HttpServer)

