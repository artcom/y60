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
ImageClone
)

foreach(TEST ${JSTESTS})
  y60_add_jstest(${TEST} y60_js)
endforeach(TEST)