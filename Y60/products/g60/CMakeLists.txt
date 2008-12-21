
ac_add_executable(
  g60
  SOURCES acgtkshell.cpp
  DEPENDS aslbase aslmath aslserial aslnet aslipc asldom aslxpath
          y60base y60scene y60image y60video y60input y60inet
          y60glrender y60glutil y60gltext y60jsbase y60jslib
          y60modelling y60sdlengine y60acgtk y60jsgtk
          aslzip
  EXTERNS GTKMM GLADEMM GTKGLEXT SPIDERMONKEY
)

