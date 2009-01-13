
set(SOURCES
AssEventSchema.cpp
AxisEvent.cpp
ButtonEvent.cpp
DebugEventSink.cpp
Event.cpp
EventDispatcher.cpp
EventRecorder.cpp
GenericEvent.cpp
KeyEvent.cpp
MouseEvent.cpp
TouchEvent.cpp
WindowEvent.cpp
asseventxsd.cpp
)

set(HEADERS
AssEventSchema.h
AxisEvent.h
ButtonEvent.h
DebugEventSink.h
Event.h
EventDispatcher.h
EventRecorder.h
GenericEvent.h
IEventSink.h
IEventSource.h
KeyCodes.h
KeyEvent.h
MouseEvent.h
TouchEvent.h
WindowEvent.h
y60_input_settings.h
)

set(TESTS
EventDispatcher
)

list( APPEND EXTERNALS OPENEXR )

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)


ac_add_library(
    y60input y60/input
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase aslmath asldom
    EXTERNS ${EXTERNALS}
    TESTS   ${TESTS}
)
