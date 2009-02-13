list( APPEND EXTERNALS ${JSEXTERN} FREETYPE SDL OPENEXR NVIDIA_CG)

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

if(Y60_MAC_OS_X_BUNDLE)
    set(Y60_APP_STYLE OSX_BUNDLE)
endif(Y60_MAC_OS_X_BUNDLE)

ac_add_executable(
  y60
  SOURCES y60.cpp
  DEPENDS aslbase aslmath aslserial aslnet aslipc asldom aslxpath
          y60base y60scene y60image y60video y60input y60inet
          y60glrender y60glutil y60gltext y60jsbase y60jslib
          y60modelling y60sdlengine ${JSDEPENDS}
          aslzip acsdlttf
  EXTERNS ${EXTERNALS}
  ${Y60_APP_STYLE}
  OSX_BUNDLE_RESOURCES Y60_RESOURCES
)


set(JSTESTS
Configurator
Dom
Enum
ErrorHandling
FileFunctions
HotCoding
JSApp
LinAlg
Locale
Modelling
MyClass
Overlay
PerlinNoise
Request
Scene
Serial
Shell
SimpleBench
UnitTest
VectorOfString
Y60JSSL
)

foreach(TEST ${JSTESTS})
  y60_add_jstest(${TEST} y60)
endforeach(TEST)
