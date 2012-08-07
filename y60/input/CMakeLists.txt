
set(SOURCES
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
GenericEventSourceFilter.cpp
asseventxsd.cpp
)

set(HEADERS
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
asseventxsd.h
GenericEventSourceFilter.h
y60_input_settings.h
)

set(TESTS
EventDispatcher
)

ac_add_library(
    y60input y60/input
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase aslmath asldom
    TESTS   ${TESTS}
    VERSION ${VERSION_STRING}
    SOVERSION ${VERSION_MAJOR}
    RUNTIME_INSTALL_COMPONENT ${RUNTIME_COMPONENT}
    DEVELOPMENT_INSTALL_COMPONENT ${DEVELOPMENT_COMPONENT}
)

asl_add_schema(
    assevent.xsd
    CXX_FILE      asseventxsd.cpp
    CXX_NAMESPACE y60
    CXX_VARNAME   ourasseventxsd
    CXX_INCLUDE   asseventxsd.h
)
