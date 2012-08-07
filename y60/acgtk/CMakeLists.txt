
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
y60_acgtk_settings.h
)

# if(OSX)
#     list(REMOVE_ITEM GTKMM_LIBRARIES "X11" "Xmu" "Xt" "ICE" "SM" "Xrender") 
#     list(REMOVE_ITEM GTKGLEXT_LIBRARIES "X11" "Xmu" "Xt" "GL" "GLU" "ICE" "SM" "Xrender")
#     message(${GTKMM_LIBRARIES})
#     message(${GTKGLEXT_LIBRARIES})
# endif(OSX)

ac_add_library(
    y60acgtk y60/acgtk
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase aslmath aslserial aslnet asldom aslxpath aslzip
            y60base y60image y60video y60inet y60scene y60input
            y60glrender y60glutil y60gltext y60modelling y60jslib
            paintlib
    EXTERNS GTKMM GTKGLEXT
    VERSION ${VERSION_STRING}
    SOVERSION ${VERSION_MAJOR}
    RUNTIME_INSTALL_COMPONENT G60_runtime
    DEVELOPMENT_INSTALL_COMPONENT G60_development
)

# fix OpenGL/X11 link cycle problem
if(OSX)
    get_property(FLAGS TARGET y60acgtk PROPERTY LINK_FLAGS)
    set_property(TARGET y60acgtk PROPERTY LINK_FLAGS "${FLAGS} -Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib")
    set(FLAGS)
endif(OSX)

