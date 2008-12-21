set(SOURCES
ACIconFactory.cpp
CellRendererPixbufToggle.cpp
ConsoleView.cpp
CustomCursors.cpp
CWRuler.cpp
DragButton.cpp
EmbeddedButton.cpp
EmbeddedToggle.cpp
GCObserver.cpp
GradientSlider.cpp
GrayScale.cpp
GtkEventAdapter.cpp
Histogram.cpp
RenderArea.cpp
testNothing.tst.cpp
TNTMeasurementList.cpp
TNTThresholdList.cpp
)

set(HEADERS
ACIconFactory.h
CellRendererPixbufToggle.h
ConsoleView.h
CustomCursors.h
CWRuler.h
DragButton.h
EmbeddedButton.h
EmbeddedToggle.h
GCObserver.h
GradientSlider.h
GrayScale.h
GtkEventAdapter.h
Histogram.h
RenderArea.h
TNTMeasurementList.h
TNTThresholdList.h
)

ac_add_library(
    y60acgtk y60/acgtk
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase aslmath aslserial aslnet asldom aslxpath aslzip
            y60base y60image y60video y60inet y60scene y60input
            y60glrender y60glutil y60gltext y60modelling y60jslib
    EXTERNS PAINTLIB SPIDERMONKEY GTKMM GTKGLEXT
)

