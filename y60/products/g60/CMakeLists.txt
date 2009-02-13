
ac_add_executable(
  g60
  SOURCES acgtkshell.cpp
  DEPENDS aslbase aslmath aslserial aslnet aslipc asldom aslxpath
          y60base y60scene y60image y60video y60input y60inet
          y60glrender y60glutil y60gltext y60jsbase y60jslib
          y60modelling y60sdlengine y60acgtk y60jsgtk
          aslzip ${JSDEPENDS}
  EXTERNS GTKMM GLADEMM GTKGLEXT ${JSEXTERN} OPENEXR
)

# fix OpenGL/X11 link cycle problem
if(OSX)
    set_property(TARGET g60 APPEND PROPERTY LINK_FLAGS " -Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib")
endif(OSX)


