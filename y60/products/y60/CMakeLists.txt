list( APPEND EXTERNALS ${JSEXTERN} FREETYPE SDL OPENEXR NVIDIA_CG)

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

ac_add_executable(
  y60
  SOURCES y60.cpp
  DEPENDS aslbase aslmath aslserial aslnet aslipc asldom aslxpath
          y60base y60scene y60image y60video y60input y60inet
          y60glrender y60glutil y60gltext y60jsbase y60jslib
          y60modelling y60sdlengine ${Y60_SDLMAIN} ${JSDEPENDS} 
          aslzip acsdlttf
  EXTERNS ${EXTERNALS}
  ${Y60_APP_STYLE}
)
